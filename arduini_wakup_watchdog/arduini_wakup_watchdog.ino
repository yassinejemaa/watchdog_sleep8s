#include <avr/sleep.h>
#include <avr/wdt.h>

int i = 0;
volatile boolean wdt_triggered = false;  // Flag to indicate watchdog wake-up
volatile boolean button_pressed = false; // Flag to indicate button press

// Watchdog Timer Interrupt Service Routine
ISR(WDT_vect) {
  wdt_triggered = true;  // Set the wake-up flag
}

void buttonISR() {
  button_pressed = true;  // Set the button press flag
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, INPUT_PULLUP); // Set pin 2 as input with pull-up

  // Attach external interrupt to pin 2
  attachInterrupt(digitalPinToInterrupt(2), buttonISR, FALLING);

  // Set up the Watchdog
  wdt_reset();  // Reset the watchdog timer
  WDTCSR = (1<<WDCE) | (1<<WDE);
  WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0); // Enable interrupt mode, set timer to 8s
}

void loop() {
  if (button_pressed || wdt_triggered) {
    if (button_pressed) {
      Serial.println("Woke up due to button press!");
      button_pressed = false;
      delay(300);
    }
    if (wdt_triggered) {
      Serial.println("Woke up due to the Watchdog Timer!");
      wdt_triggered = false;
      delay(300);
    }
    
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);

    i++;
    if (i == 8) {
      Serial.println("One minute passed");
      i = 0;
    } else {
      
    }
  }

  goToSleep();
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set the deepest sleep mode
  sleep_enable();  // Enable sleep mode
  sleep_mode();    // Go to sleep

  // The program will continue from here when the MCU wakes up
  sleep_disable(); // Disable sleep mode after wake-up
}
