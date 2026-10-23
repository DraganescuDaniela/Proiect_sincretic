#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

volatile uint8_t hours = 18, minutes = 0, seconds = 0;
volatile bool updateDisplay = false;

#define SENSOR_TMP36 1

void adcInit() {
  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adcRead(uint8_t channel) {
  ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC));
  return ADC;
}

void timer1Init() {
  cli();
  TCCR1A = 0;
TCCR1B = 0;
TCNT1 = 0;
OCR1A = 15624;
TCCR1B |= (1 << WGM12);
TCCR1B |= (1 << CS12) | (1 << CS10);
TIMSK1 |= (1 << OCIE1A);
sei();
}

ISR(TIMER1_COMPA_vect) {
if (++seconds >= 60) {
seconds = 0;
if (++minutes >= 60) {
minutes = 0;
if (++hours >= 24) hours = 0;
}
}
updateDisplay = true;
}

void setup() {
lcd.begin(16, 2);
adcInit();
timer1Init();
}

void loop() {
if (updateDisplay) {
updateDisplay = false;

    cli();
    uint8_t h = hours, m = minutes, s = seconds;
    sei();

    float mV = adcRead(0) * 5000.0 / 1024.0;
#if SENSOR_TMP36
    float temp = (mV - 500.0) / 10.0;
#else
    float temp = mV / 10.0;
#endif

    lcd.setCursor(0, 0);
    lcd.print("Temp = ");
    lcd.print((int)temp);
    lcd.print((char)223);
    lcd.print("C ");

    lcd.setCursor(0, 1);
    lcd.print("Time ");
    if (h < 10) lcd.print('0');
    lcd.print(h);
    lcd.print(':');
    if (m < 10) lcd.print('0');
    lcd.print(m);
    lcd.print(':');
    if (s < 10) lcd.print('0');
    lcd.print(s);
  }
}