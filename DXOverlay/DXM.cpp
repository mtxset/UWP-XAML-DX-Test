#include "pch.h"			// the new stdafh.h
#include "DXM.h"

using namespace Concurrency;
using namespace DXOverlay;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Core;
using namespace Platform;
using namespace Windows::Foundation;
using namespace D2D1;

static const float dpi = 96.0f;

DXSwapChainPanel::DXSwapChainPanel()
{
	rdyToRender = false;

	dxBackgroundColor = ColorF(0, 0, 0, 0);
	dxAlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	scaleX = 1.0f; scaleY = 1.0f; height = 1.0f; width = 1.0f;

	Application::Current->Suspending += ref new SuspendingEventHandler(this, &DXSwapChainPanel::OnSuspending);
	Application::Current->Resuming += ref new EventHandler<Object^>(this, &DXSwapChainPanel::OnResuming);

	this->SizeChanged += ref new SizeChangedEventHandler(this, &DXSwapChainPanel::OnSizeChanged);
	this->CompositionScaleChanged += ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DXSwapChainPanel::OnCompositionScaleChanged);
}

void DXSwapChainPanel::CreateDXFactory()
{
	D2D1_FACTORY_OPTIONS op;
	ZeroMemory(&op, sizeof(D2D1_FACTORY_OPTIONS));

	CheckRes(
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,	// mult vs single 
			__uuidof(ID2D1Factory3),			// unique ID
			&op,								// mb skip?
			&dx2Factory)
	);
}

void DXSwapChainPanel::CreateDevice()
{
	const D3D_FEATURE_LEVEL fLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	ComPtr<ID3D11Device>			dev;
	ComPtr<ID3D11DeviceContext>		devContext;

	CheckRes(
		D3D11CreateDevice(
			nullptr,							// uses first adapter in IDXGIFactory1::EnumAdapters
			D3D_DRIVER_TYPE_HARDWARE,			// hardware vs
			nullptr,							// Software
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,	// Layers and stuff
			fLevels,							// 
			ARRAYSIZE(fLevels),
			D3D11_SDK_VERSION,					// windows store apps uses d11
			&dev,
			nullptr,
			&devContext)
	);

	// D3D
	CheckRes(dev.As(&dx3Device));
	CheckRes(devContext.As(&dx3DeviceContext));

	// D2D objects
	ComPtr<IDXGIDevice3> dxgiDevice;

	CheckRes(dx3Device.As(&dxgiDevice));

	CheckRes(dx2Factory->CreateDevice(dxgiDevice.Get(), &dx2Device));
	CheckRes(dx2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dx2DeviceContext));

}

void DXSwapChainPanel::CreateDXResources()
{
	// Release objects

	dx2DeviceContext->SetTarget(nullptr);
	dx3DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	dx3DeviceContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);
	dx2Bitmap = nullptr;

	renderH = height * scaleY; renderW = width * scaleX;   //   h(y) | __ w(x)

	if (dx3SwapChain != nullptr)
	{
		HRESULT res = dx3SwapChain->ResizeBuffers(
			2,												// zwei buffers
			static_cast<UINT>(renderW),
			static_cast<UINT>(renderH),
			DXGI_FORMAT_B8G8R8A8_UNORM,						// std format ( MS )
			0);												// 0 flags for swapchain

															// reenable device
		if (res == DXGI_ERROR_DEVICE_REMOVED || res == DXGI_ERROR_DEVICE_RESET)
		{
			OnDeviceLost();
			return;
		}
		else
			CheckRes(res);
	}
	else
	{

		DXGI_SWAP_CHAIN_DESC1 ssD = { 0 };

		ssD.Width = static_cast<UINT>(renderW);
		ssD.Height = static_cast<UINT>(renderH);

		ssD.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		ssD.Stereo = false;									// fullscreen or stereo
		ssD.SampleDesc.Count = 1;							// multi-sampling
		ssD.SampleDesc.Quality = 0;							// multi-sampling
		ssD.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// DXGI_USAGE
		ssD.BufferCount = 2;								// zwei buffers
		ssD.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// ms store rules
		ssD.Flags = 0;										// DXGI_SWAP_CHAIN_FLAG
		ssD.AlphaMode = dxAlphaMode;

		ComPtr<IDXGIDevice3>	dxgiDevice;
		ComPtr<IDXGIAdapter>	dxgiAdapter;
		ComPtr<IDXGIFactory4>	dxgiFactory;
		ComPtr<IDXGISwapChain1> dxgiSwapChain;

		// getting device, adapter, factory and swapchain
		CheckRes(dx3Device.As(&dxgiDevice));
		CheckRes(dxgiDevice->GetAdapter(&dxgiAdapter));
		CheckRes(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
		CheckRes(dxgiFactory->CreateSwapChainForComposition(
			dx3Device.Get(),
			&ssD,
			nullptr,										// restrict output - nei
			&dxgiSwapChain
		));

		dxgiSwapChain.As(&dx3SwapChain);

		// frame optimization

		CheckRes(dxgiDevice->SetMaximumFrameLatency(1));

		Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]()
		{
			ComPtr<ISwapChainPanelNative> nativeSwapChain;

			CheckRes(reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&nativeSwapChain)));

			CheckRes(nativeSwapChain->SetSwapChain(dx3SwapChain.Get()));
		}, CallbackContext::Any));
	}


	// setting up proper scaling

	DXGI_MATRIX_3X2_F invScale = { 0 };

	invScale._11 = 1.0f / scaleX;
	invScale._22 = 1.0f / scaleY;

	dx3SwapChain->SetMatrixTransform(&invScale);

	// setup bitmap properties

	D2D1_BITMAP_PROPERTIES1 bitmapProps =
		BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpi * scaleX,
			dpi * scaleY
		);

	ComPtr<IDXGISurface2> dxgiBuffer;

	CheckRes(dx3SwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBuffer)));

	// setting up d2d render target

	CheckRes(dx2DeviceContext->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), &bitmapProps, &dx2Bitmap));

	dx2DeviceContext->SetDpi(dpi * scaleX, dpi * scaleY);
	dx2DeviceContext->SetTarget(dx2Bitmap.Get());
}

void DXSwapChainPanel::Present()
{
	DXGI_PRESENT_PARAMETERS params = { 0 };
	params.DirtyRectsCount = 0;
	params.pDirtyRects = nullptr;
	params.pScrollRect = nullptr;
	params.pScrollOffset = nullptr;

	HRESULT res = S_OK;

	res = dx3SwapChain->Present1(1, 0, &params);

	if (res == DXGI_ERROR_DEVICE_REMOVED || res == DXGI_ERROR_DEVICE_RESET)
		OnDeviceLost();
	else
		CheckRes(res);
}

void DXSwapChainPanel::OnDeviceLost()
{
	// reset device

	rdyToRender = false;

	dx3SwapChain = nullptr;
	dx3DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	dx2DeviceContext->SetTarget(nullptr);
	dx2Bitmap = nullptr;
	dx2DeviceContext = nullptr;
	dx2Device = nullptr;

	dx3DeviceContext->Flush();

	CreateDevice();
	CreateDXResources();
}

void DXSwapChainPanel::OnSuspending(Platform::Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ e)
{
	critical_section::scoped_lock lock(mutex);
	ComPtr<IDXGIDevice3> dxgiDevice;
	dx3Device.As(&dxgiDevice);

	dxgiDevice->Trim();
}

void DXSwapChainPanel::OnSizeChanged(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e)
{
	if (width != e->NewSize.Width || height != e->NewSize.Height)
	{
		critical_section::scoped_lock lock(mutex); 

		width = max(e->NewSize.Width, 1.0f);
		height = max(e->NewSize.Height, 1.0f);
		CreateDXResources();
	}
}

void DXSwapChainPanel::OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel ^ sender, Platform::Object ^ args)
{
	if (scaleX != CompositionScaleX || scaleY != CompositionScaleY)
	{
		critical_section::scoped_lock lock(mutex);

		scaleX = this->CompositionScaleX;
		scaleY = this->CompositionScaleY;
		CreateDXResources();
	}
}

