#include "Arduino.h"

// Global Variables

unsigned long int t_ref; // Reference time


// whenever a variable is used by both, Interrupt and main programm we
// need to tell the compiler about it ==> volatile
volatile unsigned long int ms; // milli seconds timer
char buffer[256];
unsigned int c;

void setup() {
 
  // Timer 0 konfigurieren
  TCCR0A = (1 << WGM01);               // CTC Modus (page 80)
  TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64 (page 82)
  // 16 MHz => 62.5 ns/Takt
  // Prescaler = 64 => 4 us/Takt
  // OCR = 250 ==> 4us * 250 = 1ms ==> ISR wird jede Millisekunde angesprungen
  OCR0A = 250;
  // Compare Interrupt erlauben
  TIMSK0 |= (1 << OCIE0A);
  // set pull-ups for PB0 to PB4 (we use them as INPUTS
  PORTB |= (1<<PB0 | 1<<PB1 | 1<<PB2 | 1<<PB3 | 1<<PB4);
  //PORTB = 0x1F; // would do the very same thing ...
  Serial.begin(9600);
}
void loop() {
  if (ms - t_ref >= 1000) {
    sprintf(buffer, "t=[%lu] PINB=[%2.2x]", ms,PINB);
    Serial.println(buffer);
    t_ref = ms;
  }
  // user input => get key ...
  while (Serial.available())
  {
    char c = Serial.read(); // Read a character
    sprintf(buffer, "you have pressed the [%c]-key", (char)c);
    Serial.println(buffer);
  }
}
// Timer-Interrupt-Routine
ISR(TIMER0_COMPA_vect) {
  ms++;
}
