#include "RTClib.h"     // DS3231 library
#include <avr/sleep.h>  // AVR library for controlling the sleep modes
#define sensorPin A0
#define wetSoil 277   // Define max value we consider soil 'wet'
#define drySoil 380
const int alarmPin = 2; // Pin for monitoring alarm status on DS3231
const int buttonPin = 3; // Pin connected to the button
bool i = false;
bool j = false;
RTC_DS3231 rtc;

void setup () {
  Serial.begin(9600); // Start serial port for monitoring

  pinMode(alarmPin, INPUT_PULLUP); // Set alarm pin as pullup
  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as pullup

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  rtc.writeSqwPinMode(DS3231_OFF); // Set SQW pin to alarm interrupt mode

  Serial.println("Starting");
}

void loop() {
  delay(5000);  // Wait 10 seconds before going to sleep
  if (!(i)) {Serial.println("button reset");}
  // Get current time and set alarm to a time to wake
  DateTime now = rtc.now();  // Get current time
  rtc.setAlarm1(now + TimeSpan(0, 0, 0, 10), DS3231_A1_Second); // In 10 seconds
  
  enterSleep();  // Go to sleep and wake up as per the alarm or button press
}

void enterSleep(){
  sleep_enable();                       // Enabling sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Setting the sleep mode, in this case full sleep
  
  noInterrupts(); // Disable sleep mode when awakened                  // Disable interrupts
  attachInterrupt(digitalPinToInterrupt(alarmPin), alarm_ISR, LOW); // Attach interrupt for alarm
  attachInterrupt(digitalPinToInterrupt(buttonPin), button_ISR, FALLING); // Attach interrupt for button press
  interrupts();
  Serial.println("Going to sleep!");    // Print message to serial monitor
  Serial.flush();                       // Ensure all characters are sent to the serial monitor
  
   
  sleep_cpu();                          // Enter sleep mode
  if (i){
    i= false;
    delay(50);
    Serial.println("button wakeup !");
      Serial.flush();                       // Ensure all characters are sent to the serial monitor

  }
   if (j){
    j= false;
    delay(50);
    Serial.println("RTC wakeup !");
      Serial.flush();                       // Ensure all characters are sent to the serial monitor

  }
  sleep_disable();

  // Disable and clear alarm (if awakened by alarm)
  rtc.disableAlarm(1);
  rtc.clearAlarm(1);

  Serial.println("I'm back!");
  mesuring()  ;        // Print message to show we're back
}

void alarm_ISR() {
j = true;
  detachInterrupt(digitalPinToInterrupt(alarmPin));
}

void button_ISR() {
  i = true;
  detachInterrupt(digitalPinToInterrupt(buttonPin));
}
void mesuring(){
 int moisture = analogRead(sensorPin);
  Serial.print("Analog output: ");
  Serial.println(moisture);
  
  // Determine status of our soil
  if (moisture < wetSoil) {
    Serial.println("Status: Soil is too wet");
  } else if (moisture >= wetSoil && moisture < drySoil) {
    Serial.println("Status: Soil moisture is perfect");
  } else {
    Serial.println("Status: Soil is too dry - time to water!");
  }
  Serial.println();
}

