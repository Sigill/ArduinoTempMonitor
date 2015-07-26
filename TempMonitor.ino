#include <Wire.h>
#include <SeeedGrayOLED.h>
#include <SoftwareSerial.h>

/*
 * Compute an average temparature over 15 min using this temperature sensor:
 * http://www.seeedstudio.com/wiki/Grove_-_Temperature_Sensor
 * An history over the last 24h is stored and can be browsed using a potentiometer.
 * Everything is displayed using this OLED display:
 * http://www.seeedstudio.com/wiki/index.php?title=Grove_-_OLED_Display_96*96
*/

const int D = 1000;
double currentTemperature = 0;

volatile double history[96] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// The oled screen is connected to one of the I2C ports.
// The temperature sensor is connected to the A0 port.
// The potentiometer is connected to the A1 port.

void setup()
{
  pinMode(4, OUTPUT);
  
  Wire.begin();
  SeeedGrayOled.init();             // Initialize SEEED OLED display
  SeeedGrayOled.clearDisplay();     // Clear Display.
  SeeedGrayOled.setNormalDisplay(); // Set Normal Display Mode
  SeeedGrayOled.setVerticalMode();  // Set to vertical mode for displaying text
  
  cli();
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
}

char * formatTemperature(const double T) {
  static char c[8] = { 0 };
  
  dtostrf(T, 0, 1, c);
  
  return c;
}

double getTemperature() {
  static int a;
  static double resistance;
  
  a = analogRead(A0);
  resistance = (1023 - a) * 10000./a;
  
  return 1/(log(resistance/10000.)/3975 + 1/298.15) - 273.15;
}

void pushMeasure(double measure) {
  for(int i = 95; i > 0; --i)
    history[i] = history[i-1];
  
  history[0] = measure;
}

// Timer 1 is configured to run at 1Hz.
// It will measure the temperature every minute.
// The temperature is averaged over 15 minutes, then
// stored into the 24h history.
ISR(TIMER1_COMPA_vect) {
  static char i = 0;
  static double averageTemperature = 0;
  static char count = 0;
  ++i;
  if(i == 60) { // Every 60 seconds
    if(count == 0) {
      averageTemperature = getTemperature();
    } else if(count <= 15) {
      averageTemperature += (getTemperature() - averageTemperature) / count;
    }
    
    if(count == 15) {
      currentTemperature = averageTemperature;
      pushMeasure(currentTemperature);
      count = 0;
    } else {
      ++count;
    }
    
    i = 0;
  }
}

void loop()
{
  static int a;
  static char str[13];
  
  a = analogRead(A1);
  
  SeeedGrayOled.setTextXY(0, 0);
  SeeedGrayOled.putString("Now: ");
  SeeedGrayOled.putString(formatTemperature(currentTemperature));   //Print number
  
  int h = a / 10.666667; // 1024 / 96
  
  SeeedGrayOled.setTextXY(2, 0);
  String t = String(String(h / 4) + "h" + ( (h % 4) ? String((h % 4) * 15) : "00" ) + "min ago");
  t.toCharArray(str, 13);
  if(h / 4 < 10) SeeedGrayOled.putChar('0'); // 0 padding
  SeeedGrayOled.putString(str);
  
  SeeedGrayOled.setTextXY(3, 0);
  SeeedGrayOled.putString(formatTemperature(history[h]));
  for(int i = strlen(formatTemperature(history[h])); i < 12; ++i) {
    SeeedGrayOled.putChar(' ');
  }
  
  delay(100);
}
