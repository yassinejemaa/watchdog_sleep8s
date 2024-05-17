#include <SoftwareSerial.h>
#include "RTClib.h"     // DS3231 library
#include <avr/sleep.h>  // AVR library for controlling the sleep modes
#include <OneWire.h>
#include <DallasTemperature.h>
#include<CayenneLPP.h>
#define ONE_WIRE_BUS 4
#define sensorPin A0
#define wetSoil 277   // Define max value we consider soil 'wet'
#define drySoil 380
const int alarmPin = 2; // Pin for monitoring alarm status on DS3231
const int buttonPin = 3; // Pin connected to the button
#define RX_PIN 6
#define TX_PIN 7
bool i = false;
bool j = false;
CayenneLPP lpp(0);
RTC_DS3231 rtc;
SoftwareSerial rakSerial(RX_PIN, TX_PIN);
OneWire  ds(4);
DallasTemperature sensors(&ds);

void setup () {
  Serial.begin(115200); // Start serial port for monitoring

  pinMode(alarmPin, INPUT_PULLUP); // Set alarm pin as pullup
  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as pullup

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
    sensors.begin();
  }

  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  rtc.writeSqwPinMode(DS3231_OFF); // Set SQW pin to alarm interrupt mode

  Serial.println("Starting");
  rakSerial.begin(115200);
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

  Serial.println("I'm back!");       // Print message to show we're back
  sendTemperature(mesuretemp());
}

void alarm_ISR() {
j = true;
  detachInterrupt(digitalPinToInterrupt(alarmPin));
}

void button_ISR() {
  i = true;
  detachInterrupt(digitalPinToInterrupt(buttonPin));
}
int mesuring(){
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
  return moisture;
}
float mesuretemp(){
 // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
return sensors.getTempCByIndex(0);



}
void sendTemperature(float temperature) {
  // Convert temperature to 2-byte hexadecimal string
  int tempValue = round(temperature*10); // Convert to integer (2534)
  String hexValue = String(tempValue, HEX); // Convert to hexadecimal (9E2)

  // Pad with zeros if necessary
  if (hexValue.length() < 4) {
    hexValue = "00" + hexValue;
  }

  // Construct Cayenne LPP payload in hexadecimal format
  String payload = "0067" + hexValue; // Data Type (67 for temperature), Value (9E2)

  // Construct the AT command with the payload
  String atCommand = "at+send=lora:1:" + payload;
  rakSerial.println("at+join");
  delay(2000);
  // Send the AT command
  rakSerial.println(atCommand);

  // Wait for a response
  delay(3000); // Adjust delay as needed

  // Read and print the response
  if (rakSerial.available()) {
    String response = rakSerial.readStringUntil('\n');
    Serial.println(response);
  }

  delay(250); // Adjust delay as needed
}