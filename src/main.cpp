// COMP-10184 – Mohawk College
// Alarm System Project
//
// This program is meant to act as an alarm system where the LED will start blinking once the sensor detects movement.
// The LED will keep blinking for 10 seconds once movement is detected.
// If the button on the breadboard is pressed within those 10 seconds, the LED will stop blinking and the alarm will be disabled until the button is pressed again to renable the alarm.
// If the button is not pressed within 10 seconds, the light will stay on instead of blinking and will not turn off unless the mini was restarted.
//
// @author    Wael Harba
// @id        000787960
//
// I created this work and I have not shared it with anyone else.
//

#include <Arduino.h>

// digital input pin definitions.
#define PIN_PIR D5
#define PIN_BUTTON D6

// defining all the possible alarm states.
#define ALARM_DISABLED 0
#define ALARM_ENABLE 1
#define ALARM_COUNTDOWN 2
#define ALARM_ACTIVE 3

// Defining a few time related constants.
#define BUTTON_DELAY 200     // Delay value after button is pressed.
#define BLINK_INTERVAL 125   // Interval used for LED blinking. This defines the rate blinking should happen (e.g. 125 means 125ms on time and 125 off time which equals to 250ms per blink)
#define BLINKING_TIMER 10000 // Number of ms LED should be blinking for (10 seconds).

int iAlarmState = ALARM_ENABLE;      // Global variable that will hold the state of the alarm. Alarm is enabled by default.
int iButtonState;                    // Global variable for the state of the button whether it was pressed or not.
int ledState = LOW;                  // Gobal variable used to manage the state of the LED.
unsigned long lAlarmStartMillis = 0; // Global variable to hold time in ms when the alarm countdown starts.
unsigned long lPreviousMillis = 0;   // Global variable that will store last time LED was updated by blinking.

// *************************************************************
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

  // Ensuring LED is off by default
  digitalWrite(LED_BUILTIN, true);
}

// *************************************************************
// This function is responsible for disabling the alarm by switching the alarm state to ALARM_DISABLED, resetting the countdown timer of the blinking led to 0.
// It also ensures the LED is off when alarm is disabled.
void disableAlarm()
{
  iAlarmState = ALARM_DISABLED;
  lAlarmStartMillis = 0;
  digitalWrite(LED_BUILTIN, true);
}

// *************************************************************
// This function is responsible for enabling the alarm by switching the alarm state to ALARM_ENABLE
void enableAlarm()
{
  iAlarmState = ALARM_ENABLE;
}

// *************************************************************
void loop()
{
  unsigned long lCurrentMillis = millis();     // current time since the application started
  bool bMotionDetected = digitalRead(PIN_PIR); // variable that holds the value of the PIR sensor
  iButtonState = digitalRead(PIN_BUTTON);      // store the value of the button whether it was pressed or not in iButtonState

  // Button was pressed
  if (iButtonState == 0)
  {
    delay(BUTTON_DELAY); // Delay to avoid immediately switching between alarm enabled and disabled state

    // If alarm is currently in countdown state, we disable it
    if (iAlarmState == ALARM_COUNTDOWN)
    {
      disableAlarm(); // Disable alarm
    }
    // if alarm is disabled, we enable it
    else if (iAlarmState == ALARM_DISABLED)
    {
      enableAlarm(); // Enable alarm
    }
  }

  // Check if motion was detected and the alarm is enabled
  if (bMotionDetected && iAlarmState == ALARM_ENABLE)
  {
    iAlarmState = ALARM_COUNTDOWN; // Switch alarm state to countdown since motion was detected
    lAlarmStartMillis = millis();  // Start alarm countdown timer by storing current time since app started
  }
  // Check if the alarm is in a blinking countdown state
  else if (iAlarmState == ALARM_COUNTDOWN)
  {
    // Check if it's been 10s since motion was detected and the light started blinking
    if (lCurrentMillis - lAlarmStartMillis <= BLINKING_TIMER)
    {
      // Check if it is time for the light to blink
      if (lCurrentMillis - lPreviousMillis >= BLINK_INTERVAL)
      {
        // Save the time of this blink into the lPreviousMillis variable
        lPreviousMillis = lCurrentMillis;

        ledState = (ledState == LOW) ? HIGH : LOW; // Toggle LED state variable on if it was off and vise verca

        // Set the LED light based on the ledState variable
        digitalWrite(LED_BUILTIN, ledState);
      }
    }
    else
    {
      // 10 seconds passed already and the alarm was not disabled, keep LED on unless device was restarted
      digitalWrite(LED_BUILTIN, false);
    }
  }
}