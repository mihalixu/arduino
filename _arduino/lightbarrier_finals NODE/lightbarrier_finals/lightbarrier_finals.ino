#include "Arduino.h"
#include "U8glib.h"
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

// Global Variables

unsigned long int t_ref; // Reference time
unsigned long int t_ref1; 
unsigned long int t_ref2;
unsigned long int Zeit1;
unsigned long int Zeit2;

// whenever a variable is used by both, Interrupt and main programm we
// need to tell the compiler about it ==> volatile
volatile unsigned long int ms; // milli seconds timer
char buffer[256];
unsigned int c;

#define BTN_NEXT_RACER (PB4) // button to activate time measurement for next racer
#define LB_START   (PB3) // light barrier at start
#define LB_INTERIM (PB2) // light barrier at interim
#define LB_FINISH_LINE (PB1) // light barrier at interim
#define BTN_RESET (PB0) // Reset Button

int status = 0;
#define STATUS_IDLE (0)
#define STATUS_STARTING (1) // waiting for start
#define STATUS_INRACE_1 (2) // zwischen Start und 1. Zwischenzeit
#define STATUS_INRACE_2 (3) // zwischen 1. und 2. Zwischenzeit
#define STATUS_FINISHED (4) // press NEXT-Button to return to IDLE

unsigned char PINB_last;

void draw(void) {
  u8g.setFont(u8g_font_profont12);
  u8g.setPrintPos(0, 30);
  u8g.print(buffer);
}


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

  u8g.firstPage();
  do {
    draw();
  } while (u8g.nextPage() );
  
  
  /*if (ms - t_ref >= 1000) {
      sprintf(buffer,"Current Stata: %d", status);
      Serial.println(buffer);
    t_ref = ms;
  }
 */ 
 
  switch(status)
  {
    case STATUS_IDLE:
      sprintf(buffer,"STATE: IDLE");
      if ((PINB & (1<<BTN_NEXT_RACER)) == 0 && (PINB_last & (1<<BTN_NEXT_RACER)) != 0)
        status = STATUS_STARTING;
      break;
    case STATUS_STARTING:
         sprintf(buffer,"STATE STARTING");
      if ((PINB & (1<<LB_START)) == 0 && (PINB_last & (1<<LB_START)) != 0){
        status = STATUS_INRACE_1;
        t_ref1 = ms;
      }
      break;
    case STATUS_INRACE_1:
        sprintf(buffer,"STATE: INRACE 1 %lu",ms-t_ref1);
        t_ref2 = ms;
        if ((PINB & (1<<LB_INTERIM)) == 0 && (PINB_last & (1<<LB_INTERIM)) != 0){   
         Zeit1 = ms - t_ref1;   
         status = STATUS_INRACE_2;
        }
      break;
    case STATUS_INRACE_2:
      sprintf(buffer,"STATE: INRACE 2 %lu",ms-t_ref2);
      if ((PINB & (1<<LB_FINISH_LINE)) == 0 && (PINB_last & (1<<LB_FINISH_LINE)) != 0){
        status = STATUS_FINISHED;
        Zeit2 = ms - t_ref2;   
        sprintf(buffer,"%lu %lu",Zeit1,Zeit2);
        Serial.println(buffer);
      }
      break;
    case STATUS_FINISHED:
        sprintf(buffer,"Z1: %lums Z2: %lums",Zeit1,Zeit2);
        t_ref1 = 0;
        t_ref2 = 0;
        if ((PINB & (1<<BTN_RESET)) == 0 && (PINB_last & (1<<BTN_RESET)) != 0){
            status = STATUS_IDLE;
            sprintf(buffer,"Restart");
            Serial.println(buffer);
        }         
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
      sprintf(buffer,"RESETTED!");
      Serial.println(buffer);
    }
  }
  PINB_last = PINB;
}
// Timer-Interrupt-Routine
ISR(TIMER0_COMPA_vect) {
  ms++;
}
