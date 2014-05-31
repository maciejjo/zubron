#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define START_PWM 150
#define TURN_PWM  140
#define MAX_PWM   255

#define SENSORS   7

#define K_P 15
#define K_I 2
#define K_D 1

void setup_motors();
void setup_sensors();

void motors_right();
void motors_left();
void motors_straight();

int compute_pid(int e, int pe);
int compute_error();
int is_line_visible();

int sensors[] = {
  PD1, PD2, PD3, PD0, PD4, PD5, PD6 };

int weitgts[] = {
  -3, -2, -1, 0, 1, 2, 3 };

int main()
{

  int pid_output = 0;
  int error = 0, prev_error = 0;
  int left_pwm = START_PWM, right_pwm = START_PWM;

  setup_sensors();
  setup_motors();

  while(1) {

    error = compute_error();

    motors_straight();
    if(error == 0) {

      if(is_line_visible()){
        OCR1A = TURN_PWM;
        OCR1B = TURN_PWM;
      }
      else {
        if (prev_error > 0){
          motors_right();
          OCR1A = TURN_PWM;
          OCR1B = TURN_PWM;
        }
        else {
          motors_left();
          OCR1A = TURN_PWM;
          OCR1B = TURN_PWM;
        }
      }
    }

    else {

      pid_output = compute_pid(error, prev_error);

      left_pwm = START_PWM - pid_output;
      right_pwm = START_PWM + pid_output;

      if(left_pwm > MAX_PWM){
        right_pwm += MAX_PWM - left_pwm;
        if(right_pwm < 0)
          right_pwm = 0;
        left_pwm = MAX_PWM;
      }

      if(right_pwm > MAX_PWM){
        left_pwm = left_pwm + MAX_PWM - right_pwm;
        if(left_pwm<0) left_pwm = 0;
        right_pwm = MAX_PWM;
      }

      OCR1A = right_pwm;
      OCR1B = left_pwm;

    }

    if (is_line_visible()) prev_error = error;

    _delay_ms(2);

  }
}

/**
 * Zwraca 1 jesli przynajmniej jeden czujnik wykrywa trase
 * W przeciwnym razie 0
 **/
int is_line_visible() {
  int i;
  for(i = 0; i < SENSORS; i++) {
    if(PIND & _BV(sensors[i]))
        return 1;
  }
  return 0;
}


/**
 * Zwraca błąd obliczony na podstawie tablicy wag czujników
 * (Im bardziej skrajny czujnik tym większa waga)
 **/
int compute_error() {

  int error = 0;
  int i;
  for(i = 0; i < SENSORS; i++) {
    if(PIND & _BV(sensors[i]))
        error += weitgts[i];
  }

  return error;
}

/**
 * Zwraca sumaryczną korektę ustaloną przez kontroler PID
 * (wagi członów to odpowiednio K_P, K_I i K_D)
 **/
int compute_pid(int e, int pe) {
    int p,d;
    static int i;
    p = e;
    i += e;
    d = e - pe;
    return (K_P * p) + (K_I * i) + (K_D * d);
}

/**
 * Ustawia piny odpowiedzialne za obsługę mostka H
 * (w tym także ustawienie timera 1 w tryb Fast PWM)
 **/
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

/**
 * Ustawia port do którego przyłączone są sygnały z czujników
 * jako wejściowy
 **/
void setup_sensors() {

  DDRD = 0x00;

}

/**
 * Ustawia sygnały na pinach sterujących kierunkiem silników
 * tak, aby robot skręcił w lewo
 **/
void motors_left() {

  PORTC &= ~_BV(PC0);
  PORTC |= _BV(PC1);
  PORTC |= _BV(PC2);
  PORTC &= ~_BV(PC3);

}

/**
 * Ustawia sygnały na pinach sterujących kierunkiem silników
 * tak, aby robot skręcił w prawo
 **/
void motors_right() {

  PORTC |= _BV(PC0);
  PORTC &= ~_BV(PC1);
  PORTC &= ~_BV(PC2);
  PORTC |= _BV(PC3);
}

/**
 * Ustawia sygnały na pinach sterujących kierunkiem silników
 * tak, aby robot jechał prosto
 **/
void motors_straight() {

  PORTC &= ~_BV(PC0);
  PORTC |= _BV(PC1);
  PORTC &= ~_BV(PC2);
  PORTC |= _BV(PC3);

}
