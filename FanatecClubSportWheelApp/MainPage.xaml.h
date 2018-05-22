//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include <vector>

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

        ConditionForceEffect ^ m_springEffect;
        std::vector <RacingWheel^> m_racingWheels;

        void displayFFcaps(RacingWheel^ racingwheel);
        void testSpringEffectFor(RacingWheel^ racingwheel);
        void StartTimerAndRegisterHandler();
        void OnTick(Object^ sender, Object^ e);
	};
}
