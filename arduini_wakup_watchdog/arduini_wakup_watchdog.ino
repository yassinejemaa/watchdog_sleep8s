#include <avr/sleep.h>
#include <avr/wdt.h>
int i=0 ;

volatile boolean wdt_triggered = false;  // Flag to indicate watchdog wake-up

// Watchdog Timer Interrupt Service Routine
ISR(WDT_vect) {
  wdt_triggered = true;  // Set the wake-up flag
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // Set up the Watchdog
  wdt_reset();  // Reset the watchdog timer
  // Set watchdog timer to wake up after approximately 8 seconds
  WDTCSR = (1<<WDCE) | (1<<WDE);
  WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0); // Enable interrupt mode, set timer to 8s g
}

void loop() {
  i++;
  if (i==8){
    //place to put the code of every cycle , read data from sensor , just testing
    Serial.println("one minute passed");
    i=0;
    goToSleep();
  }
  else{
    goToSleep();
    Serial.println(i);
    if (wdt_triggered) {  // Check if wake-up was due to the Watchdog Timer
    Serial.println("Woke up due to the Watchdog Timer!");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    wdt_triggered = false;  // Reset the flag
   }
  }
}

void goToSleep() {

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set the deepest sleep mode
  sleep_enable();  // Enable sleep mode
  sleep_mode();    // Go to sleep

  // The program will continue from here when the MCU wakes up
  sleep_disable(); // Disable sleep mode after wake-up
  
  
  
}
