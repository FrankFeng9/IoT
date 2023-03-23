
// AlarmSystem
// This program represents an alarm system. When montion detected, alarm countdown starts. User can deactive the alarm by pressing the button.
// If the button is not pressed within 10s, the alarm will be triggered. The alarm can be reset by pressing the button.


#include <Arduino.h>
// digital input pin definitions
#define PIN_PIR D5
#define PIN_BUTTON D6
// define all possible alarm states.
#define ALARM_DISABLED 0
#define ALARM_ENABLE 1
#define ALARM_COUNTDOWN 2
#define ALARM_ACTIVE 3
//define the button debounce time
#define BUTTON_DELAY 200
//define the LED off state to HIGH
#define LED_OFF HIGH 
//define the numbers of led blinks in countdown state
#define BLINK_COUNT 40
//define delay time between each led on/off
#define BLINK_INTERVAL 125

int iAlarmState = ALARM_ENABLE;  // initial alarm state
bool bPIR = false;               // PIR sensor detection flag  initial value is false
int iButton;                     // button state pressed or not
int count = 0;                   // counter for led blinking times


void setup()
{
    // configure the USB serial monitor
    Serial.begin(115200);
    // configure the LED output
    pinMode(LED_BUILTIN, OUTPUT);
    // PIR sensor is an INPUT
    pinMode(PIN_PIR, INPUT);
    // Button is an INPUT
    pinMode(PIN_BUTTON, INPUT_PULLUP);
}


// collegeinput function -- collectie inputs from the sensor and button
void collectInputs()
{
    //reading PIR sensor
    bPIR = digitalRead(PIN_PIR);
    //reading button press action
    iButton = digitalRead(PIN_BUTTON);
}

// Check alarm state function. It monitors the alarm state and change the state accordingly.
//led is used to indicate the alarm countdown and alarm active state.

void checkAlarmState()
{
    switch (iAlarmState)
    {
    case ALARM_DISABLED:
        // if the alarm is disabled, press the button to enable the alarm

        // set the led off as the initial state
        digitalWrite(LED_BUILTIN, LED_OFF);

        // if the button is pressed, enable the alarm
        if (iButton == LOW)
        {
            delay(BUTTON_DELAY);
            iAlarmState = ALARM_ENABLE;
            Serial.println("Alarm Enabled");
        }
        break;

    case ALARM_ENABLE:
        // if the alarm is enabled, sensor detects motion, start the alarm countdown.

        // set the led off as the initial state
        Serial.println("Alarm armed");
        digitalWrite(LED_BUILTIN, LED_OFF);

        if (bPIR)    //motion detected
        {
            iAlarmState = ALARM_COUNTDOWN;
            Serial.println("Motion Detected! Alarm Countdown Starts!");
        }
        break;

    case ALARM_COUNTDOWN:
        //if the button is not pressed during countdown, the led blink 4 times per second for 10 seconds.

        //when button is not pressed. Count each led on/off action, 80 times in total
        while (iButton == HIGH && count < BLINK_COUNT *2 )   
        {
            collectInputs();                                       //check button press action during the countdown
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // led on/off to blink
            delay(125);                                            // delay 125ms between each on/off
            count++;                                              
        }

        //if the button is pressed, stop the countdown and disable the alarm
        if (iButton == LOW)
        {
            delay(BUTTON_DELAY);
            iAlarmState = ALARM_DISABLED;
            digitalWrite(LED_BUILTIN, LED_OFF);
            Serial.println("Alarm disabled!");
        }else{
            // if the button is not pressed during count down, the alarm is triggered
            iAlarmState = ALARM_ACTIVE;
            Serial.println("Alarm Triggered!");
            break;
        }
              
    case ALARM_ACTIVE:
         //alarm active state, the led is always on
        digitalWrite(LED_BUILTIN, !LED_OFF);
        break;
    }
}

void loop()
{
    //get button and motion detector inputs
    collectInputs();
    //check alarm state, and change the state accordingly
    checkAlarmState();
}
