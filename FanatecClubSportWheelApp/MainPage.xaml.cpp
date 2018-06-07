//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//


#include "pch.h"
#include "MainPage.xaml.h"
#include <iostream>
#include <string>
#include <WindowsNumerics.h>
#include <sstream>

using namespace std;

using namespace FanatecClubSportWheelApp;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Gaming::Input;
using namespace Windows::Gaming::Input::ForceFeedback;
using namespace Windows::Networking::Connectivity;


// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
: m_winsockData()
{
    m_rcvTimePoint1 = m_rcvTimePoint2;

    createInitConditionForceParams();
 
	InitializeComponent();

    initializeHostIpPortNoUI();
    
    refreshMyIpTextBox();

    cout << "Initializing network connection ..." << endl;

    if (!initializeWinsock())
    {
        // Create the message dialog and set its content
        const wchar_t* errMsgWchar = m_winsockData.errMsg.c_str();
        Platform::String^ errMsg = ref new Platform::String(errMsgWchar);

        MessageDialog^ msg = ref new MessageDialog(errMsg);

        UICommand^ closeCommand = ref new UICommand(
            "Close",
            ref new UICommandInvokedHandler(this, &FanatecClubSportWheelApp::MainPage::CloseCommandInvokedHandler));
        msg->Commands->Append(closeCommand);
        msg->ShowAsync();
    }

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

void FanatecClubSportWheelApp::MainPage::CloseCommandInvokedHandler(Windows::UI::Popups::IUICommand ^ command)
{
    Application::Current->Exit();
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

        receiveDataFromSocket();
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

bool FanatecClubSportWheelApp::MainPage::initializeWinsock()
{
    bool initialized(true);

    do 
    {
        cout << "Initialising Winsock ..." << endl;

        if (WSAStartup(MAKEWORD(2, 2), &m_winsockData.wsaData) != 0)
        {
            cout << "ERROR:: " << endl;
            cout << "ERROR:: WSAStartup() failed with error: " << WSAGetLastError() << endl;
            cout << "ERROR:: " << endl;

            m_winsockData.errMsg.append(L"ERROR:: WSAStartup failed with error: ");
            std::wstring iResult = std::to_wstring(WSAGetLastError());
            m_winsockData.errMsg.append(iResult);
            m_winsockData.errMsg.append(L"\n");

            WSACleanup();

            initialized = false;
            break; // exit do-while(ONE_TIME) loop
        }
        cout << "Winsock initialised ." << endl;

        // create socket
        cout << "Creating UDP socket ..." << endl;
        if ((m_winsockData.recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
        {
            cout << "ERROR:: " << endl;
            cout << "ERROR:: socket() failed with error: " << WSAGetLastError() << endl;
            cout << "ERROR:: " << endl;

            m_winsockData.errMsg.append(L"ERROR:: socket() failed with error: ");
            std::wstring iResult = std::to_wstring(WSAGetLastError());
            m_winsockData.errMsg.append(iResult);
            m_winsockData.errMsg.append(L"\n");

            WSACleanup();

            initialized = false;
            break; // exit do-while(ONE_TIME) loop
        }
        cout << "socket created." << endl;

        u_long iMode = 1;
        ioctlsocket(m_winsockData.recvSocket, FIONBIO, &iMode);

        //Prepare the sockaddr_in structure
        m_winsockData.recvAddr.sin_family = AF_INET;
        m_winsockData.recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        // m_winsockData.server.sin_addr.s_addr = inet_addr("192.168.100.130");

        // Update PORT  
        auto port(ToULong(portNumberBox->Text));
        m_winsockData.recvAddr.sin_port = htons(port);

        cout << "Using Port : " << port << endl << endl;

        // Bind
        if (::bind(m_winsockData.recvSocket, 
                (struct sockaddr *)&m_winsockData.recvAddr, 
sizeof(m_winsockData.recvAddr)) == SOCKET_ERROR)
        {
        printf("Bind failed with error code : %d", WSAGetLastError());
        cout << "ERROR:: " << endl;
        cout << "ERROR:: Bind() failed with error: " << WSAGetLastError() << endl;
        cout << "ERROR:: " << endl;

        m_winsockData.errMsg.append(L"ERROR:: Bind() failed with error: ");
        std::wstring iResult = std::to_wstring(WSAGetLastError());
        m_winsockData.errMsg.append(iResult);
        m_winsockData.errMsg.append(L"\n");

        WSACleanup();

        initialized = false;
        break; // exit do-while(ONE_TIME) loop
        }

cout << "Bind done" << endl;

    } while (ONE_TIME);

    return initialized;
}

void FanatecClubSportWheelApp::MainPage::resetWinsock()
{
    cout << "Resetting winsock ..." << endl;
    closesocket(m_winsockData.recvSocket);
    WSACleanup();
}

void FanatecClubSportWheelApp::MainPage::initializeHostIpPortNoUI()
{
    std::ostringstream s;
    s << m_winsockData.port;

    std::string portStr = s.str();
    std::wstring portWidStr = std::wstring(portStr.begin(), portStr.end());
    const wchar_t* portWchar = portWidStr.c_str();
    Platform::String^ portString = ref new Platform::String(portWchar);

    portNumberBox->Text = portString;
}

void FanatecClubSportWheelApp::MainPage::networkResetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    resetWinsock();

    if (!initializeWinsock())
    {
        // Create the message dialog and set its content
        const wchar_t* errMsgWchar = m_winsockData.errMsg.c_str();
        Platform::String^ errMsg = ref new Platform::String(errMsgWchar);

        MessageDialog^ msg = ref new MessageDialog(errMsg);

        /* UICommand^ closeCommand = ref new UICommand(
        "Close",
        ref new UICommandInvokedHandler(this, &FanatecClubSportWheelApp::MainPage::CloseCommandInvokedHandler));
        msg->Commands->Append(closeCommand);
        */
        msg->ShowAsync();
    }
}

void FanatecClubSportWheelApp::MainPage::refreshMyIpTextBox()
{
    for each (auto localHostName in NetworkInformation::GetHostNames())
    {

        if (localHostName->IPInformation)
        {
            if (localHostName->Type == Windows::Networking::HostNameType::Ipv4)
            {
                myIpTextBox->Text = localHostName->ToString();
                break;
            }
        }
    }
}

bool FanatecClubSportWheelApp::MainPage::receiveDataFromSocket()
{
    static bool firstTimeCalled(true);
    static const double MAXTIME_ELAPSE_SECS(5.0);

    if (firstTimeCalled)
    {
        firstTimeCalled = false;
        resetRcvTimePoints();
    }

    bool ok(true);

    std::chrono::duration<double> time_span;

    // Clear the buffer by filling null, it might have previously received data
    memset(m_winsockData.recvBuf, '\0', WinsockConst::BUFLEN);

    // try to receive some data, this is a blocking call
    if ((m_winsockData.recv_len =
        ::recvfrom(
            m_winsockData.recvSocket,
            m_winsockData.recvBuf,
            m_winsockData.bufLen,
            0,
            (struct sockaddr *) &m_winsockData.senderAddr,
            &m_winsockData.senderAddrSize)) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
        {
            m_rcvTimePoint2 = std::chrono::high_resolution_clock::now();
            time_span = m_rcvTimePoint2 - m_rcvTimePoint1;

            if (time_span.count() > MAXTIME_ELAPSE_SECS)
            {
                resetRcvTimePoints();
                cout << "time_span.count: " << time_span.count() << " seconds." << endl;

                cout << "ERROR:: " << endl;
                cout << "ERROR:: recvfrom() - Client not sending data. " << endl;
                cout << "ERROR:: " << endl;
            }
            
            ok = false;
        }

        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            cout << "ERROR:: " << endl;
            cout << "ERROR:: recvfrom() failed with error code: " << WSAGetLastError() << endl;
            cout << "ERROR:: " << endl;

            m_winsockData.errMsg.append(L"ERROR:: recvfrom() failed with error code: ");
            std::wstring iResult = std::to_wstring(WSAGetLastError());
            m_winsockData.errMsg.append(iResult);
            m_winsockData.errMsg.append(L"\n");

            ok = false;
        }
        
    }

    if (ok)
    {
        resetRcvTimePoints();
        
        // Print details of the client/peer and the data received
        cout << "Received packet from " << inet_ntoa(m_winsockData.senderAddr.sin_addr) << ":"
             << ntohs(m_winsockData.senderAddr.sin_port) << endl << endl;
        cout << "Data: " << m_winsockData.recvBuf << endl;             
    }

    return ok;
}

void FanatecClubSportWheelApp::MainPage::resetRcvTimePoints()
{
    m_rcvTimePoint1 = std::chrono::high_resolution_clock::now();
    m_rcvTimePoint2 = m_rcvTimePoint1;
}

unsigned long FanatecClubSportWheelApp::MainPage::ToULong(Platform::String^ str)
{
    const wchar_t* begin = str->Data();
    return std::wcstoul(begin, nullptr, 10);
}

int FanatecClubSportWheelApp::MainPage::testServer()
{
    int iResult = 0;

    WSADATA wsaData;

    SOCKET RecvSocket;
    sockaddr_in RecvAddr;

    unsigned short Port = 8844;

    char RecvBuf[1024];
    int BufLen = 1024;

    sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);

    //-----------------------------------------------
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error %d\n", iResult);
        return 1;
    }
    //-----------------------------------------------
    // Create a receiver socket to receive datagrams
    RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (RecvSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error %d\n", WSAGetLastError());
        return 1;
    }
    //-----------------------------------------------
    // Bind the socket to any address and the specified port.
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(Port);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    iResult = ::bind(RecvSocket, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
    if (iResult != 0) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        return 1;
    }
    wprintf(L"bind works.\n");
    
    //-----------------------------------------------
    // Call the recvfrom function to receive datagrams
    // on the bound socket.
    wprintf(L"Waiting to Receive datagrams...\n");
    iResult = recvfrom(RecvSocket,
        RecvBuf, BufLen, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
    }
    // Print details of the client/peer and the data received
    cout << "Received packet from " << inet_ntoa(SenderAddr.sin_addr) << ":"
        << ntohs(SenderAddr.sin_port) << endl << endl;
    cout << "Data: " << RecvBuf << endl;

    //-----------------------------------------------
    // Close the socket when finished receiving datagrams
    wprintf(L"Finished receiving. Closing socket.\n");
    iResult = closesocket(RecvSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
        return 1;
    }

    //-----------------------------------------------
    // Clean up and exit.
    wprintf(L"Exiting.\n");
    WSACleanup();
    return 0;
}