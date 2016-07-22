#include "pch.h"
#include "RendM.h"


using namespace DXOverlay;
using namespace Platform;
using namespace D2D1;

using namespace Concurrency;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;



RendM::RendM() : lastXpos(0), lastYpos(0)
{
	CreateDXFactory();
	CreateDevice();
	CreateDXResources();
}

void RendM::CreateDevice()
{
	DXSwapChainPanel::CreateDevice();

	dx2DeviceContext->CreateSolidColorBrush(ColorF(ColorF::BlueViolet), &dxLineBrush);
	dx2DeviceContext->SetUnitMode(D2D1_UNIT_MODE::D2D1_UNIT_MODE_PIXELS);

	rdyToRender = true;
}

void RendM::DrawFace()
{
	auto center = D2D1::Point2F(renderW / 2, renderH / 2);
	auto leftEyePos = D2D1::Point2F(center.x - 30, center.y - 50);
	auto rightEyePos = D2D1::Point2F(center.x + 30, center.y - 50);


	dx2DeviceContext->Clear(dxBackgroundColor);
	dx2DeviceContext->Clear(ColorF(0, 0, 0, 0));

	// draw Face
	ID2D1SolidColorBrush *outlineBrush;
	ID2D1SolidColorBrush *faceBrush;
	dx2DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &outlineBrush);
	dx2DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &faceBrush);
	D2D1_ELLIPSE face = D2D1::Ellipse(center, 100, 100);
	dx2DeviceContext->FillEllipse(face, faceBrush);
	dx2DeviceContext->DrawEllipse(face, outlineBrush);

	// draw Eyes
	D2D1_ELLIPSE leftEye = D2D1::Ellipse(leftEyePos, 10, 30);
	D2D1_ELLIPSE rightEye = D2D1::Ellipse(rightEyePos, 10, 30);
	dx2DeviceContext->FillEllipse(leftEye, outlineBrush);
	dx2DeviceContext->FillEllipse(rightEye, outlineBrush);
}

void RendM::DrawRain()
{
	dx2DeviceContext->CreateSolidColorBrush(ColorF(ColorF::Black), &RainBrush);
	float dist = 20.f;
	float w = 0.3f, h = 20.f;

	float incX = 0.01f, incY = 1.5f;

	float pos1[] = { 0, 0 };
	float pos2[] = { 0, 0 };

	for (int i = 0; i < dropCount; i++)
	{ 
		dx[i] += incX; dy[i] += incY;

		pos1[0] = lastXpos + dist + dx[i];
		pos1[1] = lastYpos + dist + dy[i];

		pos2[0] = lastXpos + dist + w + dx[i];
		if ((dist + h + dy[i]) <= 100.0)
			pos2[1] = lastYpos + dist + h + dy[i];
		else
			pos2[1] = lastYpos + dist + h;

		if (pos1[1] >= pos2[1]) dx[i] = (float)(1.0f * (rand() % 100)), dy[i] = (float)(1.0f * (rand() % 100));

		D2D1_RECT_F rect = RectF(pos1[0], pos1[1], pos2[0], pos2[1]);

		dx2DeviceContext->FillRectangle(rect, RainBrush);
	}

	
	
}

void RendM::DrawSun()
{
	float time = float(clock() - SunBeginTime);
	SunRadius = SunAmp * sin(SunFreq * time * 3.14) / exp2f(SunDecay*time);

	float r = SunRadius + SunInitRadius;
	D2D1_ELLIPSE sun = Ellipse(Point2F(lastXpos + 30, lastYpos + 30), r, r);
	dx2DeviceContext->FillEllipse(sun, SunBrush);
}

void RendM::Render()
{	
		if (!rdyToRender)
		{
			return;
		}

		
		dx2DeviceContext->BeginDraw();
		dx2DeviceContext->Clear(ColorF(0, 0, 0, 0));

		//if (RenderSun) DrawSun();
		if (RenderRain) DrawRain();

		dx2DeviceContext->EndDraw();

		Present();
}

void RendM::OnDeviceLost()
{
	DXSwapChainPanel::OnDeviceLost();
	Render();
}
 
int RendM::testVal()
{
	return 27314; // epie
}


void RendM::InitSunDraw()
{
	dx2DeviceContext->CreateSolidColorBrush(ColorF(ColorF::Goldenrod), &SunBrush);

	SunFreq = 2;
	SunAmp = 25;
	SunDecay = 0.0001f;
	SunRadius = 0;
	SunInitRadius = 50;
	clock_t SunBeginTime = clock();

	RenderSun = true;
}

void RendM::InitRainDraw(int drops)
{
	srand(time(NULL));
	dropCount = drops;
	if (drops > 100) dropCount = 100;

	for (int i = 0; i < dropCount; i++)
		//dx[i] = 0.f, dy[i] = 0.f;
		dx[i] = (float)(1.0f * (rand() % 100)), dy[i] = (float)(1.0f * (rand() % 100));

	RenderRain = true;
}

void DXOverlay::RendM::DisSunDraw()
{
	RenderSun = false;
}

void DXOverlay::RendM::DisRainDraw()
{
	RenderRain = false;
}

void RendM::RenderRoutine(float x, float y)
{
	lastXpos = x; lastYpos = y;
	Render();
}


void RendM::OnSizeChanged(Object ^ sender, SizeChangedEventArgs ^ e)
{
	DXSwapChainPanel::OnSizeChanged(sender, e);
	Render();
}

void RendM::OnCompositionScaleChanged(SwapChainPanel ^ sender, Object ^ args)
{
	DXSwapChainPanel::OnCompositionScaleChanged(sender, args);
	Render();
}

void RendM::OnResuming(Object ^ sender, Object ^ args)
{
	Render();
}

