#include <Arduino.h>
// WE WANT TO READ FROM PB4 (DIGITAL INPUT)
// USING INTERNAL PULL_UP ==> ACTIVATION = LOW

#define SETBIT(REG,POS) (REG |= (1<<POS))
#define ISSET(REG,POS) ((REG & (1<<POS))==0)
#define RESETBIT(REG,POS) (REG &= ~(1<<POS))

void setup() {
// xxxx xxxx 
// 1) DEFINE PB4 AS INPUT (DDRB is to be manipulated)
// 2) PB5 (LED) AS OUTPUT
//DDRB |= (1<<5) | (1<<1); // PB5 und PB1 otuput, all others are inputs
DDRB |= SETBIT(DDRB,5) | SETBIT(DDRB,1);
//PORTB |= (1<<4) | (1<<2); // turns on PULL-UP of PB4 und PB2
PORTB |= SETBIT(PORTB,4) | SETBIT(PORTB,2);
}

void loop() {

  if (ISSET(PINB,4) && ISSET(PINB,2))
    SETBIT(PORTB,5);
  else
    RESETBIT(PORTB,5);
 
}
