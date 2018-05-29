﻿//
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

        struct DamperEffectConst
        {
            const float MIN_FEEDBACK_FORCE = 0.001f;
        };
        DamperEffectConst DAMPER_EFFECT_CONST;

        struct FrictionEffectConst
        {
            const float MIN_FEEDBACK_FORCE = 0.001f;
        };
        FrictionEffectConst FRICTION_EFFECT_CONST;

        struct InertiaEffectConst
        {
            const float MIN_FEEDBACK_FORCE = 0.001f;
        };
        InertiaEffectConst INERTIA_EFFECT_CONST;

        struct ConditionForceParams
        {
            float strenghtAtMaxAngle; // strength when the wheel is turned to its maximum angle. Range is 0.0 to 1.0. full=1.0
            float maxFeedbackForce;   // Limit the maximum feedback force. Range is 0 to 1.0. 0.30 is 30%
            float centeredDeadZone;   // Dead zone when the wheel is centered. Range 0 .. 1.0
            float bias;               // The offset to the center point in effect calculations. Range is from -1.0 to 1.0. 0.0 is Equal force in both directions
        
            ConditionForceParams()
                : strenghtAtMaxAngle(0.0), maxFeedbackForce(0.0), centeredDeadZone(0.0), bias(0.0)
            {}
        };

        std::array<ConditionForceParams*, 4> m_conditionForceParams;

        ConditionForceEffect ^ m_springEffect;
        ConditionForceEffect ^ m_damperEffect;
        ConditionForceEffect ^ m_frictionEffect;
        ConditionForceEffect ^ m_inertiaEffect;

        std::vector <RacingWheel^> m_racingWheels;

        void createInitConditionForceParams();

        void displayFFcaps(RacingWheel^ racingwheel);

        void updateDisplay();

        void StartTimerAndRegisterHandler();
        void OnTick(Object^ sender, Object^ e);

        // Damper Effect stuff
        void CreateLoadDamperEffectFor(RacingWheel^ racingwheel);
        void SetDamperEffectParameters();

        void damperMaxFeedbackForceSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void damperForceAtMaxAngleSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void damperDeadZoneSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void damperBiasSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void damperEnableButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        // Spring Effect stuff
        void CreateLoadSpringEffectFor(RacingWheel^ racingwheel);
        void SetSpringEffectParameters();

        void springMaxFeedbackForceSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void springForceAtMaxAngleSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void springDeadZoneSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void springBiasSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void springEnableButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        // Friction effect stuff
        void CreateLoadFrictionEffectFor(RacingWheel^ racingwheel);
        void SetFrictionEffectParameters();

        void frictionMaxFeedbackForceSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void frictionForceAtMaxAngleSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void frictionDeadZoneSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void frictionBiasSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void frictionEnableButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        // Inertia effect stuff
        void CreateLoadInertiaEffectFor(RacingWheel^ racingwheel);
        void SetInertiaEffectParameters();

        void inertiaMaxFeedbackForceSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void inertiaForceAtMaxAngleSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void inertiaDeadZoneSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void inertiaBiasSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void inertiaEnableButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    };
}
