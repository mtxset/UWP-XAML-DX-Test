#pragma once
#include "pch.h"
#include "DXM.h"
using namespace std;
using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace DXOverlay
{
	public ref class RendM sealed : public DXSwapChainPanel
    {
    public:

        RendM();

		int testVal();

		void InitSunDraw();
		void InitRainDraw(int drops);

		void DisSunDraw();
		void DisRainDraw();

		void RenderRoutine(float x, float y);
		// methods
	private protected:

			virtual void CreateDevice() override;

			virtual void Render() override;

		// Event handlers

			virtual void OnSizeChanged(Object^ s, SizeChangedEventArgs^ e) override;
			virtual void OnCompositionScaleChanged(SwapChainPanel ^s, Object ^args) override;
			virtual void OnResuming(Object^ s, Platform::Object^ args) override;
			
			virtual void OnDeviceLost() override;

		//.Event handlers

		// Drawing methods

			void DrawFace();

			void DrawRain();
			void DrawSun();

		//.Draw

		//.methods

		// params
			
			// Rain Params
			float dx[100], dy[100];
			ID2D1SolidColorBrush* RainBrush;
			int dropCount;
			bool RenderRain = false;
			
			//.Rain Params

			// Sun Params

			int SunFreq;
			int SunAmp;
			float SunDecay;
			float SunRadius;
			float SunInitRadius;
			clock_t SunBeginTime;
			ID2D1SolidColorBrush* SunBrush;

			bool RenderSun = false;

			//.Sun
			
			ComPtr<ID2D1SolidColorBrush> dxLineBrush;
			ComPtr<ID2D1SolidColorBrush> dxFillBrush;

		//.params

		// other
			float lastXpos, lastYpos;
		
    };
}
