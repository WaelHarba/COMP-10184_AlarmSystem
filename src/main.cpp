// COMP-10184 – Mohawk College
// Alarm System Project
//
// Stages Completed: All stages are completed.
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
unsigned long lCurrentMillis;        // Global variable for the time since the application started.
bool bMotionDetected;                // Global variable for the value of the PIR sensor.

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
// This function is responsible for getting all the required values for the alarm system functionality and it stores these values
// in the appropriate global variables.
void collectInputs()
{
  lCurrentMillis = millis();              // Store the value of the time since the application started in lCurrentMillis
  bMotionDetected = digitalRead(PIN_PIR); // Read and store the value of the PIR sensor whether a movement was detected or not in bMotionDetected
  iButtonState = digitalRead(PIN_BUTTON); // Read and store the value of the button whether it was pressed or not in iButtonState
}

// *************************************************************
// This function is responsible for enabling or disabling the alarm when the pushbutton is pressed.
// It disables the alarm when it's in a countdown state by doing the following 3 things:
// switching the state of the iAlarmState variable to ALARM_DISABLED, Resetting countdown timer by setting lAlarmStartMillis to 0, and turning off the LED.
// It enables the alarm only by switching the iAlarmState variable to ALARM_ENABLE.
//
void enableOrDisableAlarm()
{
  // Button was pressed
  if (iButtonState == 0)
  {
    delay(BUTTON_DELAY); // Delay to avoid immediately switching between alarm enabled and disabled state

    // If alarm is currently in countdown state, we disable it
    if (iAlarmState == ALARM_COUNTDOWN)
    {
      iAlarmState = ALARM_DISABLED;    // Switch alarm state to disabled
      lAlarmStartMillis = 0;           // Reset time for when countdown timer started to 0
      digitalWrite(LED_BUILTIN, true); // Turn off the LED since the alarm was disabled
    }
    // if alarm is disabled, we enable it
    else if (iAlarmState == ALARM_DISABLED)
    {
      iAlarmState = ALARM_ENABLE; // Switch the alarm state to enabled
    }
  }
}

// *************************************************************
// This function will help in the blinking functionality. It turns the LED on if it was off and vice versa.
// It also stores the time of the blink to help knowing when to blink next.
void blink()
{
  lPreviousMillis = lCurrentMillis; // Save the time of this blink into the lPreviousMillis variable

  ledState = (ledState == LOW) ? HIGH : LOW; // Toggle LED state variable on if it was off and vice versa

  digitalWrite(LED_BUILTIN, ledState); // Set the LED light based on the ledState variable
}

// *************************************************************
// This function is responsible for starting the alarm if the alarm is enabled and motion is detected.
// If the alarm started, the LED starts blinking 4 times per second for 10 seconds.
// If 10 seconds pass and the user doesn't disable the alarm, the alarm will be activated by keeping LED on always unless the device is restarted.
void checkAlarmState()
{
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
        blink(); // blink the LED
      }
    }
    else
    {
      // 10 seconds passed already and the alarm was not disabled, keep LED on unless device was restarted
      digitalWrite(LED_BUILTIN, false);
    }
  }
}

// *************************************************************
void loop()
{
  // Get all the inputs we need for the alarm functionality (motion detector value, pushbutton state value, and time since alarm was enabled)
  collectInputs();

  // Checking if button was pressed, and enabling/disabling if button was pressed depending on alarm state
  enableOrDisableAlarm();

  // check and switch alarm state if needed
  checkAlarmState();
}