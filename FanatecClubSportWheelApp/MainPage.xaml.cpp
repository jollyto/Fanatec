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
            CreateLoadDamperEffectFor(m_racingWheels[0]);
            CreateLoadFrictionEffectFor(m_racingWheels[0]);
            CreateLoadInertiaEffectFor(m_racingWheels[0]);

            updateDisplay();
        }

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
    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce = 0.30f;   // 0.30 = 30%
    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->strenghtAtMaxAngle = 1.0f;  // 100%

}

void FanatecClubSportWheelApp::MainPage::updateDisplay()
{
    // Update spring UI controls
    float val(m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce);
    springMaxFeedbackForceTextBox->Text = (val * 100.0f).ToString() + "%";
    springMaxFeedbackForceSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->bias;
    springBiasTextBox->Text = (val * 100.0f).ToString() + "%";
    springBiasSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->centeredDeadZone;
    springDeadZoneTextBox->Text = (val * 100.0f).ToString() + "%";
    springDeadZoneSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->strenghtAtMaxAngle;
    springForceAtMaxAngleTextBox->Text = (val * 100.0f).ToString() + "%";
    springForceAtMaxAngleSlider->Value = static_cast<int>(val * 100.0f);

    // Update damper UI controls
    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->maxFeedbackForce;
    damperMaxFeedbackForceTextBox->Text = (val * 100.0f).ToString() + "%";
    damperMaxFeedbackForceSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->bias;
    damperBiasTextBox->Text = (val * 100.0f).ToString() + "%";
    damperBiasSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->centeredDeadZone;
    damperDeadZoneTextBox->Text = (val * 100.0f).ToString() + "%";
    damperDeadZoneSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->strenghtAtMaxAngle;
    damperForceAtMaxAngleTextBox->Text = (val * 100.0f).ToString() + "%";
    damperForceAtMaxAngleSlider->Value = static_cast<int>(val * 100.0f);

    // Update friction UI controls
    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->maxFeedbackForce;
    frictionMaxFeedbackForceTextBox->Text = (val * 100.0f).ToString() + "%";
    frictionMaxFeedbackForceSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->bias;
    frictionBiasTextBox->Text = (val * 100.0f).ToString() + "%";
    frictionBiasSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->centeredDeadZone;
    frictionDeadZoneTextBox->Text = (val * 100.0f).ToString() + "%";
    frictionDeadZoneSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->strenghtAtMaxAngle;
    frictionForceAtMaxAngleTextBox->Text = (val * 100.0f).ToString() + "%";
    frictionForceAtMaxAngleSlider->Value = static_cast<int>(val * 100.0f);

    // Update inertia UI controls
    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->maxFeedbackForce;
    inertiaMaxFeedbackForceTextBox->Text = (val * 100.0f).ToString() + "%";
    inertiaMaxFeedbackForceSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->bias;
    inertiaBiasTextBox->Text = (val * 100.0f).ToString() + "%";
    inertiaBiasSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->centeredDeadZone;
    inertiaDeadZoneTextBox->Text = (val * 100.0f).ToString() + "%";
    inertiaDeadZoneSlider->Value = static_cast<int>(val * 100.0f);

    val = m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->strenghtAtMaxAngle;
    inertiaForceAtMaxAngleTextBox->Text = (val * 100.0f).ToString() + "%";
    inertiaForceAtMaxAngleSlider->Value = static_cast<int>(val * 100.0f);
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

void FanatecClubSportWheelApp::MainPage::SetDamperEffectParameters()
{
    bool wasRunning(false);

    if (m_damperEffect->State == ForceFeedbackEffectState::Running)
    {
        m_damperEffect->Stop();
        wasRunning = true;
    }

    if (m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->maxFeedbackForce <= 0.0)
    {
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->maxFeedbackForce = DAMPER_EFFECT_CONST.MIN_FEEDBACK_FORCE;
    }

    // Set the parameters for the damper effect.  Note how the parameters
    // can be modified after the effect has been loaded into the hardware.
    m_damperEffect->SetParameters(

        // Unit vector indicating the effect applies to the X axis
        float3(1.0f, 0.0f, 0.0f),

        // strength when the wheel is turned to its maximum angle (full =1.0) 
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->strenghtAtMaxAngle,
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->strenghtAtMaxAngle,

        // Limit the maximum feedback force to x%
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->maxFeedbackForce,
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->maxFeedbackForce,

        // Apply a small dead zone when the wheel is centered
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->centeredDeadZone,

        // Equal force in both directions. Bias = 0
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->bias
    );

    if (wasRunning)
    {
        m_damperEffect->Start();
    }

}

void FanatecClubSportWheelApp::MainPage::SetSpringEffectParameters()
{
    bool wasRunning(false);

    if (m_springEffect->State == ForceFeedbackEffectState::Running)
    {
        m_springEffect->Stop();
        wasRunning = true;
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

    if (wasRunning)
    {
        m_springEffect->Start();
    }
    
}

void FanatecClubSportWheelApp::MainPage::CreateLoadDamperEffectFor(RacingWheel ^ racingwheel)
{
    if (racingwheel->WheelMotor != nullptr)
    {
        using FFLoadEffectResult = ForceFeedback::ForceFeedbackLoadEffectResult;
        
        m_damperEffect = ref new Windows::Gaming::Input::ForceFeedback::ConditionForceEffect(ConditionForceEffectKind::Damper);

        if (m_damperEffect)
        {

            IAsyncOperation<FFLoadEffectResult>^ request
                = racingwheel->WheelMotor->LoadEffectAsync(m_damperEffect);

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
                    cout << "Damper effect successfully loaded" << endl;

                    this->SetDamperEffectParameters();
                }
                else
                {
                    cout << "Damper effect failed to load" << endl;
                }
            });

        }

    }
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

                    this->SetSpringEffectParameters();
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

void FanatecClubSportWheelApp::MainPage::damperBiasSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(damperBiasSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->bias = val;

    damperBiasTextBox->Text = (val*100.0f).ToString() + "%";

    SetDamperEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::damperMaxFeedbackForceSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(damperMaxFeedbackForceSlider->Value) * 0.01f);

    // cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->maxFeedbackForce = val;

    damperMaxFeedbackForceTextBox->Text = (val*100.0f).ToString() + "%";

    SetDamperEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::damperForceAtMaxAngleSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(damperForceAtMaxAngleSlider->Value) * 0.01f);

    // cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->strenghtAtMaxAngle = val;

    damperForceAtMaxAngleTextBox->Text = (val*100.0f).ToString() + "%";

    SetDamperEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::damperDeadZoneSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(damperDeadZoneSlider->Value) * 0.01f);

    // cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Damper)]->centeredDeadZone = val;

    damperDeadZoneTextBox->Text = (val*100.0f).ToString() + "%";

    SetDamperEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::damperEnableButton_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (m_damperEffect)
    {
        if (m_damperEffect->State == ForceFeedbackEffectState::Running)
        {
            m_damperEffect->Stop();
            damperEnableButton->Content = "Enable";
        }
        else
        {
            m_damperEffect->Start();
            damperEnableButton->Content = "Disable";
        }
    }
}

void FanatecClubSportWheelApp::MainPage::springMaxFeedbackForceSlider_ValueChanged(
    Platform::Object^ sender, 
    Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    float val(static_cast<float>(springMaxFeedbackForceSlider->Value) * 0.01f);

    //cout << "val: " << val << endl; 

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->maxFeedbackForce = val;

    springMaxFeedbackForceTextBox->Text = (val*100.0f).ToString() + "%";

    SetSpringEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::springForceAtMaxAngleSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(springForceAtMaxAngleSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->strenghtAtMaxAngle = val;

    springForceAtMaxAngleTextBox->Text = (val*100.0f).ToString() + "%";

    SetSpringEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::springDeadZoneSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(springDeadZoneSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->centeredDeadZone = val;

    springDeadZoneTextBox->Text = (val*100.0f).ToString() + "%";

    SetSpringEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::springBiasSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(springBiasSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Spring)]->bias = val;

    springBiasTextBox->Text = (val*100.0f).ToString() + "%";

    SetSpringEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::springEnableButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (m_springEffect)
    {
        if (m_springEffect->State == ForceFeedbackEffectState::Running)
        {
            m_springEffect->Stop();
            springEnableButton->Content = "Enable";
        }
        else
        {
            m_springEffect->Start();
            springEnableButton->Content = "Disable";
        }
    }
}

void FanatecClubSportWheelApp::MainPage::CreateLoadFrictionEffectFor(RacingWheel ^ racingwheel)
{
    if (racingwheel->WheelMotor != nullptr)
    {
        using FFLoadEffectResult = ForceFeedback::ForceFeedbackLoadEffectResult;

        m_frictionEffect = ref new Windows::Gaming::Input::ForceFeedback::ConditionForceEffect(ConditionForceEffectKind::Friction);

        if (m_frictionEffect)
        {

            IAsyncOperation<FFLoadEffectResult>^ request
                = racingwheel->WheelMotor->LoadEffectAsync(m_frictionEffect);

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
                    cout << "Friction effect successfully loaded" << endl;

                    this->SetFrictionEffectParameters();
                }
                else
                {
                    cout << "Friction effect failed to load" << endl;
                }
            });

        }

    }
}

void FanatecClubSportWheelApp::MainPage::SetFrictionEffectParameters()
{
    bool wasRunning(false);

    if (m_frictionEffect->State == ForceFeedbackEffectState::Running)
    {
        m_frictionEffect->Stop();
        wasRunning = true;
    }

    if (m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->maxFeedbackForce <= 0.0)
    {
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->maxFeedbackForce = FRICTION_EFFECT_CONST.MIN_FEEDBACK_FORCE;
    }

    // Set the parameters for the friction effect.  Note how the parameters
    // can be modified after the effect has been loaded into the hardware.
    m_frictionEffect->SetParameters(

        // Unit vector indicating the effect applies to the X axis
        float3(1.0f, 0.0f, 0.0f),

        // strength when the wheel is turned to its maximum angle (full =1.0) 
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->strenghtAtMaxAngle,
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->strenghtAtMaxAngle,

        // Limit the maximum feedback force to x%
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->maxFeedbackForce,
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->maxFeedbackForce,

        // Apply a small dead zone when the wheel is centered
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->centeredDeadZone,

        // Equal force in both directions. Bias = 0
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->bias
    );

    if (wasRunning)
    {
        m_frictionEffect->Start();
    }

}

void FanatecClubSportWheelApp::MainPage::frictionMaxFeedbackForceSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(frictionMaxFeedbackForceSlider->Value) * 0.01f);

    //cout << "val: " << val << endl; 

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->maxFeedbackForce = val;

    frictionMaxFeedbackForceTextBox->Text = (val*100.0f).ToString() + "%";

    SetFrictionEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::frictionForceAtMaxAngleSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(frictionForceAtMaxAngleSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->strenghtAtMaxAngle = val;

    frictionForceAtMaxAngleTextBox->Text = (val*100.0f).ToString() + "%";

    SetFrictionEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::frictionDeadZoneSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(frictionDeadZoneSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->centeredDeadZone = val;

    frictionDeadZoneTextBox->Text = (val*100.0f).ToString() + "%";

    SetFrictionEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::frictionBiasSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(frictionBiasSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Friction)]->bias = val;

    frictionBiasTextBox->Text = (val*100.0f).ToString() + "%";

    SetFrictionEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::frictionEnableButton_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (m_frictionEffect)
    {
        if (m_frictionEffect->State == ForceFeedbackEffectState::Running)
        {
            m_frictionEffect->Stop();
            frictionEnableButton->Content = "Enable";
        }
        else
        {
            m_frictionEffect->Start();
            frictionEnableButton->Content = "Disable";
        }
    }
}

void FanatecClubSportWheelApp::MainPage::CreateLoadInertiaEffectFor(RacingWheel ^ racingwheel)
{
    if (racingwheel->WheelMotor != nullptr)
    {
        using FFLoadEffectResult = ForceFeedback::ForceFeedbackLoadEffectResult;

        m_inertiaEffect = ref new Windows::Gaming::Input::ForceFeedback::ConditionForceEffect(ConditionForceEffectKind::Inertia);

        if (m_inertiaEffect)
        {

            IAsyncOperation<FFLoadEffectResult>^ request
                = racingwheel->WheelMotor->LoadEffectAsync(m_inertiaEffect);

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
                    cout << "Inertia effect successfully loaded" << endl;

                    this->SetInertiaEffectParameters();
                }
                else
                {
                    cout << "Inertia effect failed to load" << endl;
                }
            });

        }

    }
}

void FanatecClubSportWheelApp::MainPage::SetInertiaEffectParameters()
{
    bool wasRunning(false);

    if (m_inertiaEffect->State == ForceFeedbackEffectState::Running)
    {
        m_inertiaEffect->Stop();
        wasRunning = true;
    }

    if (m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->maxFeedbackForce <= 0.0)
    {
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->maxFeedbackForce = INERTIA_EFFECT_CONST.MIN_FEEDBACK_FORCE;
    }

    // Set the parameters for the spring effect.  Note how the parameters
    // can be modified after the effect has been loaded into the hardware.
    m_inertiaEffect->SetParameters(

        // Unit vector indicating the effect applies to the X axis
        float3(1.0f, 0.0f, 0.0f),

        // strength when the wheel is turned to its maximum angle (full =1.0) 
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->strenghtAtMaxAngle,
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->strenghtAtMaxAngle,

        // Limit the maximum feedback force to x%
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->maxFeedbackForce,
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->maxFeedbackForce,

        // Apply a small dead zone when the wheel is centered
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->centeredDeadZone,

        // Equal force in both directions. Bias = 0
        m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->bias
    );

    if (wasRunning)
    {
        m_inertiaEffect->Start();
    }

}

void FanatecClubSportWheelApp::MainPage::inertiaMaxFeedbackForceSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(inertiaMaxFeedbackForceSlider->Value) * 0.01f);

    //cout << "val: " << val << endl; 

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->maxFeedbackForce = val;

    inertiaMaxFeedbackForceTextBox->Text = (val*100.0f).ToString() + "%";

    SetInertiaEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::inertiaForceAtMaxAngleSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(inertiaForceAtMaxAngleSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->strenghtAtMaxAngle = val;

    inertiaForceAtMaxAngleTextBox->Text = (val*100.0f).ToString() + "%";

    SetInertiaEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::inertiaDeadZoneSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(inertiaDeadZoneSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->centeredDeadZone = val;

    inertiaDeadZoneTextBox->Text = (val*100.0f).ToString() + "%";

    SetInertiaEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::inertiaBiasSlider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e)
{
    float val(static_cast<float>(inertiaBiasSlider->Value) * 0.01f);

    //cout << "val: " << val << endl;

    m_conditionForceParams[static_cast<int>(ConditionForceEffectKind::Inertia)]->bias = val;

    inertiaBiasTextBox->Text = (val*100.0f).ToString() + "%";

    SetInertiaEffectParameters();
}

void FanatecClubSportWheelApp::MainPage::inertiaEnableButton_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (m_inertiaEffect)
    {
        if (m_inertiaEffect->State == ForceFeedbackEffectState::Running)
        {
            m_inertiaEffect->Stop();
            inertiaEnableButton->Content = "Enable";
        }
        else
        {
            m_inertiaEffect->Start();
            inertiaEnableButton->Content = "Disable";
        }
    }
}