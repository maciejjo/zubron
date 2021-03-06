#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define SENSORS 7
#define START_PWM 200

void setup_motors();
void motors_right();
void motors_left();
void motors_straight();
void toggle_led();
int check_all();

int compute_error();


int sensors[] = {
  PD1, PD2, PD3, PD0, PD4, PD5, PD6 };

int weitgts[] = {
  -3, -2, -1, 0, 1, 2, 3 };

int kp = 15;
int ki = 0;
int kd = 0;
int tp = START_PWM;

int errors[5] = { 0, 0, 0, 0, 0};

int main()
{
  DDRB |= _BV(PB0);
  DDRD = 0x00;

  setup_motors();

  int change_pwm = 0;
  int prev_error = 0;
  int error = 0;
  int p = 0,i = 0,d = 0;


  while(1) {

    prev_error = error;
    error = compute_error();


    if(check_all()) {
      for(int x = 0; x < 4; x++) {
        errors[x] = errors[x+1];
      }
      errors[4] = error;
    }


    toggle_led();

    if(!check_all()) {

      int r_count = 0, l_count = 0;
      for(int i = 0; i < 5; i++) {
        if(errors[i] < 0)
          l_count++;
        if(errors[i] > 0)
          r_count++;
      }


      if(l_count > r_count)
        motors_left();
      else
        motors_right();
      OCR1A = 255;
      OCR1B = 255;
    }

    else {

      p = error;

      change_pwm = kp*p;

      int right = tp + change_pwm;
      int left = tp - change_pwm;
      if(right > 255) right = 255;
      if(left > 255) left = 255;

      motors_straight();

      OCR1A = right;
      OCR1B = left;


    }
    _delay_ms(5);

  }
}

void toggle_led() {

    if(PIND & _BV(PD0))
      PORTB |= _BV(PB0);
    else
      PORTB &= ~_BV(PB0);

}

int check_all() {
  int result = 0;
  int i;
  for(i = 0; i < SENSORS; i++) {
    if(PIND & _BV(sensors[i]))
        result = 1;
  }
  return result;
}


int compute_error() {

  int error = 0;
  int i;
  for(i = 0; i < SENSORS; i++) {
    if(PIND & _BV(sensors[i]))
        error += weitgts[i];
  }

  return error;
}

void setup_motors() {

  DDRC |= _BV(PC0);
  DDRC |= _BV(PC1);
  DDRC |= _BV(PC2);
  DDRC |= _BV(PC3);


  TCCR1A = (_BV(COM1A1) | _BV(COM1B1));
  OCR1A = START_PWM;
  OCR1B = START_PWM;
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
