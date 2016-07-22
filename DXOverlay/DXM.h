//#pragma once
#include <concrt.h>

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml;


namespace DXOverlay
{
	public ref class DXSwapChainPanel : public Windows::UI::Xaml::Controls::SwapChainPanel
	{
		inline void CheckRes(HRESULT res)
		{
			if (!SUCCEEDED(res))
				throw Exception::CreateException(res);
		}

	protected private:

		// DX stuff

		ComPtr<ID3D11Device3>			dx3Device;
		ComPtr<ID3D11DeviceContext3>	dx3DeviceContext;
		ComPtr<IDXGISwapChain3>			dx3SwapChain;

		ComPtr<ID2D1Factory3>			dx2Factory;
		ComPtr<ID2D1Device>				dx2Device;
		ComPtr<ID2D1DeviceContext>		dx2DeviceContext;
		ComPtr<ID2D1Bitmap1>			dx2Bitmap;

		D2D1_COLOR_F					dxBackgroundColor; // will be set to 0000 for transparency
		DXGI_ALPHA_MODE					dxAlphaMode;

		float							renderH, renderW, scaleX, scaleY, height, width;
		//.DX 

		// Methods
			
			DXSwapChainPanel();

			void			CreateDXFactory();
			virtual void	CreateDevice();
			void			CreateDXResources();

			virtual void Render() {};
			virtual void Present();

		//.Methods

		// Events
			
			virtual void OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e);
			virtual void OnCompositionScaleChanged(SwapChainPanel ^sender, Object ^args);
			virtual void OnSuspending(Object^ sender, SuspendingEventArgs^ e);
			virtual void OnResuming(Object^ sender, Object^ args) { };

			virtual void OnDeviceLost();

		//.Events

		// Other

			bool								rdyToRender;
			Concurrency::critical_section		mutex;
	};
}