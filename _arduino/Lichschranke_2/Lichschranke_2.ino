
void setup() {
  DDRB |= (1<<PB5);
  PORTD |= (1<<PD2);
  PORTB |= (1<<PB2);
  Serial.begin(9600);
}
void loop() {
     if((PIND & (1<<PD2)) == 0){
        PORTB |= (1<<PB5);
     }
     else if((PINB & (1<<PB2)) == 0){
        PORTB &= ~(1<<PB5);
     }    
}
