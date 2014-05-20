#include <avr/io.h>
#include <util/delay.h>

void setup_motors();
void motors_right();
void motors_left();
void motors_straight();


int sensors[] = {
  PD1, PD2, PD3, PD0, PD4, PD5, PD6 };


int main()
{
  DDRB |= _BV(PB0);
  DDRD = 0x00;

  setup_motors();


  while(1) {
    if(PIND & _BV(PD0))
      motors_straight();
    else {
      if(PIND & _BV(PD3))
        motors_left();

      if(PIND & _BV(PD4))
        motors_right();
    }
    _delay_ms(100);

  }
}

void setup_motors() {

  DDRC |= _BV(PC0);
  DDRC |= _BV(PC1);
  DDRC |= _BV(PC2);
  DDRC |= _BV(PC3);


  TCCR1A = (_BV(COM1A1) | _BV(COM1B1));
  OCR1A = 200;
  OCR1B = 200;
  TCCR1A |= _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10);
  DDRB |= (_BV(PB1) | _BV(PB2));

}

void motors_left() {

  PORTC &= ~_BV(PC0);
  PORTC |= _BV(PC1);

  PORTC &= ~_BV(PC3);
  PORTC &= ~_BV(PC2);


}

void motors_right() {

  PORTC &= ~_BV(PC0);
  PORTC &= ~_BV(PC1);

  PORTC &= ~_BV(PC2);
  PORTC |= _BV(PC3);
}

void motors_straight() {

  PORTC &= ~_BV(PC0);
  PORTC |= _BV(PC1);

  PORTC &= ~_BV(PC2);
  PORTC |= _BV(PC3);

}
