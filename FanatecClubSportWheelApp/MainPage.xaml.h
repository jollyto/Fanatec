//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include <vector>
#include <array>

using namespace Windows::Gaming::Input;
using namespace Windows::Gaming::Input::ForceFeedback;

namespace FanatecClubSportWheelApp
{
  
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

    private:
  
        struct SpringEffectConst
        {
            const float MIN_FEEDBACK_FORCE = 0.001f;
        };
        SpringEffectConst SPRING_EFFECT_CONST;

        struct ConditionForceParams
        {
            float strenghtAtMaxAngle; // strength when the wheel is turned to its maximum angle. Range is 0.0 to 1.0. full=1.0
            float maxFeedbackForce;   // Limit the maximum feedback force. Range is 0 to 1.0. 0.30 is 30%
            float centeredDeadZone;   // Apply a small dead zone when the wheel is centered. 0 .. 1.0
            float bias;               // The offset to the center point in effect calculations. Range is from -1.0 to 1.0. 0.0 is Equal force in both directions
        
            ConditionForceParams()
                : strenghtAtMaxAngle(0.0), maxFeedbackForce(0.0), centeredDeadZone(0.0), bias(0.0)
            {}
        };

        std::array<ConditionForceParams*, 4> m_conditionForceParams;

        ConditionForceEffect ^ m_springEffect;
        ConditionForceEffect ^ m_damperEffect;

        std::vector <RacingWheel^> m_racingWheels;

        void createInitConditionForceParams();

        void displayFFcaps(RacingWheel^ racingwheel);
        void CreateLoadSpringEffectFor(RacingWheel^ racingwheel);
        
        void SetSpringEffectsParameters();

        void StartTimerAndRegisterHandler();
        void OnTick(Object^ sender, Object^ e);
        void springMaxFeedbackForceSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
    };
}
