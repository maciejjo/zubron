#include <avr/io.h>
#include <util/delay.h>

int main()
{
  DDRB |= _BV(PB0);
  //DDRB |= _BV(PB1);
  //DDRB |= _BV(PB2);

  //PORTB |= _BV(PB1);
  //PORTB |= _BV(PB2);

  DDRC |= _BV(PC0);
  DDRC |= _BV(PC1);
  DDRC |= _BV(PC2);
  DDRC |= _BV(PC3);

  PORTC |= _BV(PC1);
  PORTC |= _BV(PC3);

  PORTC &= ~_BV(PC0);
  PORTC &= ~_BV(PC2);

  OCR2 = 128;
  TCCR2 |= (1 << COM21);
  TCCR2 |= (1 << WGM21) | (1 << WGM20);
  TCCR2 |= (1 << CS21);

  while(1) {
    PORTB ^= _BV(PB0);
    _delay_ms(100);
  }
}
