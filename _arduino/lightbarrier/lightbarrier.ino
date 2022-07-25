#include "Arduino.h"

// Global Variables

unsigned long int t_ref; // Reference time
unsigned long int t_ref1; 
unsigned long int t_ref2;

// whenever a variable is used by both, Interrupt and main programm we
// need to tell the compiler about it ==> volatile
volatile unsigned long int ms; // milli seconds timer
char buffer[256];
unsigned int c;

#define BTN_NEXT_RACER (PB4) // button to activate time measurement for next racer
#define LB_START   (PB3) // light barrier at start
#define LB_INTERIM (PB2) // light barrier at interim
#define LB_FINISH_LINE (PB1) // light barrier at interim

char status = 0;
#define STATUS_IDLE (0)
#define STATUS_STARTING (1) // waiting for start
#define STATUS_INRACE_1 (2) // zwischen Start und 1. Zwischenzeit
#define STATUS_INRACE_2 (3) // zwischen 1. und 2. Zwischenzeit
#define STATUS_FINISHED (4) // press NEXT-Button to return to IDLE

unsigned char PINB_last;
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
  PORTD |= (1<<PD2);
  //PORTB = 0x1F; // would do the very same thing ...
  Serial.begin(9600);
}
void loop() {
  if (ms - t_ref >= 1000) {
    sprintf(buffer, "t=[%lu] PINB=[%2.2x] status=[%d]", ms,PINB,status);
    Serial.println(buffer);
    t_ref = ms;
  }
  switch(status)
  {
    case STATUS_IDLE:
      if ((PINB & (1<<BTN_NEXT_RACER)) == 0 && (PINB_last & (1<<BTN_NEXT_RACER)) != 0)
        status = STATUS_STARTING;
      break;
    case STATUS_STARTING:
      if ((PINB & (1<<LB_START)) == 0 && (PINB_last & (1<<LB_START)) != 0){
        status = STATUS_INRACE_1;
        t_ref1 = ms;
      }
      break;
    case STATUS_INRACE_1:
        t_ref2 = ms;
        if ((PINB & (1<<LB_INTERIM)) == 0 && (PINB_last & (1<<LB_INTERIM)) != 0){      
         status = STATUS_INRACE_2;
         sprintf(buffer,"Zwischezeit 1. = [%lu]",ms - t_ref1); 
         Serial.println(buffer);
        }
      break;
    case STATUS_INRACE_2:
      if ((PINB & (1<<LB_FINISH_LINE)) == 0 && (PINB_last & (1<<LB_FINISH_LINE)) != 0){
        status = STATUS_FINISHED;
        sprintf(buffer,"Zwischezeit 2. = [%lu]",ms - t_ref2); 
        Serial.println(buffer); 
      }
      break;
    case STATUS_FINISHED:
      if(c=='r' or c== 'R')
        t_ref1 = 0;
        t_ref2 = 0;
        status = STATUS_IDLE;
      break; 
  }
  
  // user input => get key ...
  while (Serial.available())
  {
    char c = Serial.read(); // Read a character
//    sprintf(buffer, "you have pressed the [%c]-key", (char)c);
//    Serial.println(buffer);
    if (c == 'r' || c == 'R')
    {
      status = 0;
      sprintf(buffer,"WHOLE THING RESETTED BY USER!");
      Serial.println(buffer);
    }
  }
  PINB_last = PINB;
}
// Timer-Interrupt-Routine
ISR(TIMER0_COMPA_vect) {
  ms++;
}
