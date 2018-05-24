//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <iostream>
#include <string>
#include <WindowsNumerics.h>

using namespace std;

using namespace FanatecClubSportWheelApp;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Gaming::Input;
using namespace Windows::Gaming::Input::ForceFeedback;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
    createInitConditionForceParams();
 
	InitializeComponent();

    int count(0);

    cout << "Waiting to connect to wheel ..." << endl;

    while (RacingWheel::RacingWheels->Size < 2);
    Sleep(2000);
    for (auto racingWheel : RacingWheel::RacingWheels)
    {
        m_racingWheels.push_back(racingWheel);

        /*cout << "RacingWheel[" << count << "]" << endl;
        cout << "Name : ";
        cout << racingWheel->ToString()->Data() << endl;

        cout << "HasClutch ...: " << string(racingWheel->HasClutch ? "YES" : "NO") << endl;
        cout << "HasHandbrake : " << string(racingWheel->HasHandbrake ? "YES" : "NO") << endl;

        cout << endl;
        count++;*/

    }

    if (m_racingWheels.size() > 0)
    {
        cout << "Name ...........: ";
        cout << m_racingWheels[0]->ToString()->Data() << endl;
        cout << "maxWheelDegrees : " << m_racingWheels[0]->MaxWheelAngle << endl;
        cout << "FF Supported ...: " << string((m_racingWheels[0]->WheelMotor != nullptr) ? "YES" : "NO") << endl;

        if (m_racingWheels[0]->WheelMotor != nullptr)
        {
            // force feedback is supported
            displayFFcaps(m_racingWheels[0]);
            CreateLoadSpringEffectFor(m_racingWheels[0]);
        }

        /*while (true)
        {
        cout << "angle: " << racingWheels[0]->GetCurrentReading().Wheel << endl;
        }*/
    }

    StartTimerAndRegisterHandler();

    cout << "\nDone!" << endl;
}

void FanatecClubSportWheelApp::MainPage::createInitConditionForceParams()
{
    // Create Initialize condition force parameters
    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]   = new ConditionForceParams();
    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)] = new ConditionForceParams();
    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]  = new ConditionForceParams();
    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]   = new ConditionForceParams();

    // Set Spring default values
    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce   = SPRING_EFFECT_CONST.MIN_FEEDBACK_FORCE; // 0.30 = 30%
    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->strenghtAtMaxAngle = 1.0f;  // 100%

}

void FanatecClubSportWheelApp::MainPage::displayFFcaps(RacingWheel^ racingwheel)
{
    if (racingwheel->WheelMotor != nullptr)
    {
        auto axes = racingwheel->WheelMotor->SupportedAxes; 

        cout << "Force can be applied through the X axis: " << string((ForceFeedbackEffectAxes::X == (axes & ForceFeedbackEffectAxes::X)) ? "YES" : "NO") << endl;
        cout << "Force can be applied through the Y axis: " << string((ForceFeedbackEffectAxes::Y == (axes & ForceFeedbackEffectAxes::Y)) ? "YES" : "NO") << endl;
        cout << "Force can be applied through the Z axis: " << string((ForceFeedbackEffectAxes::Z == (axes & ForceFeedbackEffectAxes::Z)) ? "YES" : "NO") << endl;
    }
}

void FanatecClubSportWheelApp::MainPage::SetSpringEffectsParameters()
{
    if (m_springEffect->State == ForceFeedbackEffectState::Running)
    {
        m_springEffect->Stop();
    }
    
    if (m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce <= 0.0)
    {
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce = SPRING_EFFECT_CONST.MIN_FEEDBACK_FORCE;
    }

    // Set the parameters for the spring effect.  Note how the parameters
    // can be modified after the effect has been loaded into the hardware.
    m_springEffect->SetParameters(

        // Unit vector indicating the effect applies to the X axis
        float3(1.0f, 0.0f, 0.0f),

        // strength when the wheel is turned to its maximum angle (full =1.0) 
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->strenghtAtMaxAngle,
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->strenghtAtMaxAngle,

        // Limit the maximum feedback force to x%
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce,
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce,

        // Apply a small dead zone when the wheel is centered
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->centeredDeadZone,

        // Equal force in both directions. Bias = 0
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->bias
    );

    m_springEffect->Start();
}

void FanatecClubSportWheelApp::MainPage::CreateLoadSpringEffectFor(RacingWheel^ racingwheel)
{
    
    if (racingwheel->WheelMotor != nullptr)
    {
        using FFLoadEffectResult = ForceFeedback::ForceFeedbackLoadEffectResult;

        m_springEffect = ref new Windows::Gaming::Input::ForceFeedback::ConditionForceEffect(ConditionForceEffectKind::Spring);

        if (m_springEffect)
        {

            IAsyncOperation<FFLoadEffectResult>^ request
                = racingwheel->WheelMotor->LoadEffectAsync(m_springEffect);

            auto loadEffectTask = Concurrency::create_task(request);

            loadEffectTask.then([this](FFLoadEffectResult result)
            {
                // Make sure the effect was loaded successfully.  There is a finite amount
                // of storage available for effects in the hardware, so this is expected
                // to fail if there is not enough room.  Alternatively, the motor might
                // not support the requested effect (although this is rare).
                //
                if (FFLoadEffectResult::Succeeded == result)
                {
                    cout << "Spring effect successfully loaded" << endl;

                    this->SetSpringEffectsParameters();
                }
                else
                {
                    cout << "Spring effect failed to load" << endl;
                }
            });

        }

    }
}

void FanatecClubSportWheelApp::MainPage::StartTimerAndRegisterHandler()
{
    auto timer = ref new Windows::UI::Xaml::DispatcherTimer();
    TimeSpan ts;
    ts.Duration = 500;
    timer->Interval = ts;
    timer->Start();
    auto registrationtoken = timer->Tick += ref new EventHandler<Object^>(this, &MainPage::OnTick);
}

void FanatecClubSportWheelApp::MainPage::OnTick(Object ^ sender, Object ^ e)
{
    auto maxAnglePerSide(m_racingWheels[0]->MaxWheelAngle / 2.0f);
    double currentAngle(0);
    static double prevAngle(-1.0);
    double displayAngle(0);

    if (m_racingWheels[0])
    {
        currentAngle = m_racingWheels[0]->GetCurrentReading().Wheel;

        if (currentAngle != prevAngle)
        {
            displayAngle = currentAngle * maxAnglePerSide;
            cout << "angle: (" << currentAngle << ") "
                 << displayAngle << " degrees." << endl;
            prevAngle = currentAngle;

            angleTextBox->Text = displayAngle.ToString();
        }
    }
}


void FanatecClubSportWheelApp::MainPage::springMaxFeedbackForceSlider_ValueChanged(
    Platform::Object^ sender, 
    Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    float val(static_cast<float>(springMaxFeedbackForceSlider->Value) * 0.01f);

    cout << "val: " << val << endl; 

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce = val;

    springFFBValueTextBox->Text = (val*100.0f).ToString() + "%";

    SetSpringEffectsParameters();
}
