#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define START_PWM 150
#define SENSORS 7

void setup_motors();
void motors_right();
void motors_left();
void motors_straight();
int compute_error();
int check_all();

int sensors[] = {
  PD1, PD2, PD3, PD0, PD4, PD5, PD6 };

int weitgts[] = {
  -3, -2, -1, 0, 1, 2, 3 };

int kp = 15; //30
int ki = 1;  //1
int kd = 1;  //0

int main()
{
  DDRB |= _BV(PB0);
  DDRD = 0x00;

  setup_motors();
  int error = 0;
  int change_pwm = 0;
  int prev_error = 0;
  int left = START_PWM;
  int right = START_PWM;
  int p = 0;
  int i = 0;
  int d = 0;
  //motors_straight();
  while(1) {
    error = compute_error();
    p = error * kp;
    d = error - prev_error;
    i += error;
    change_pwm = p + ki*i + kd*d;
    motors_straight();
    if(error == 0){
      //PORTB |= _BV(PB0);
      if(check_all()){
        OCR1A = 140;
        OCR1B = 140;
        //motors_straight();
      }
      else {
        if (prev_error > 0){
          //motors_left();
          motors_right();
          OCR1A = 140;
          OCR1B = 140;
          //motors_right();
        }
        else {
          //motors_right();
          motors_left();
          OCR1A = 140;
          OCR1B = 140;
          //motors_left();
        }
      }
    }
    else {
      PORTB &= ~_BV(PB0);
      left = START_PWM - change_pwm;
      right = START_PWM + change_pwm;
      if(left > 255){
        right = right + 255 - left;
        if(right<0) right = 0;
        left = 255;
      }
      if(right > 255){
        left = left + 255 - right;
        if(left<0) left = 0;
        right = 255;
      }
      OCR1A = right; //zmieniona predkosc podana na silniki
      OCR1B = left; 
    }
    if (check_all()) prev_error = error;
      /*if(!((PIND & _BV(PD3)) && (PIND & _BV(PD4)))){
        if(PIND & _BV(PD3)){
          OCR1A=240;
          OCR1B=160;
        }
          // motors_left();

        if(PIND & _BV(PD4)){
          OCR1A=160;
          OCR1B=240;
          // motors_right();
        }
      }
      if(PIND & _BV(PD1))
          motors_left();
      if(PIND & _BV(PD6))
          motors_right();
    }*/
    
    _delay_ms(2);

  }
}


int check_all() {
  int i;
  for(i = 0; i < SENSORS; i++) {
    if(PIND & _BV(sensors[i]))
        return 1;
  }
  return 0;
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

  DDRC |= _BV(PC0);//ustawienie jako wyjscie pinow "silnikowych"
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

  PORTC &= ~_BV(PC0); //silnik ustawiony w tryb jazdy (0,1)
  PORTC |= _BV(PC1);

  PORTC |= _BV(PC2); //silnik w trybie stopu(0,0)
  PORTC &= ~_BV(PC3);

}

void motors_right() {

  PORTC |= _BV(PC0);//silnik w trybie stopu(0,0)
  PORTC &= ~_BV(PC1);

  PORTC &= ~_BV(PC2);//silnik w trybie jazdy(0,1)
  PORTC |= _BV(PC3);
}

void motors_straight() {

  PORTC &= ~_BV(PC0);//oba silniki "w przod"
  PORTC |= _BV(PC1);
  
  PORTC &= ~_BV(PC2);
  PORTC |= _BV(PC3);

  OCR1A = START_PWM; //predkosc
  OCR1B = START_PWM;
}
