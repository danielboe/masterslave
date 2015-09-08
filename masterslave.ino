//test
#include <EEPROM.h>
#include <Wire.h>    // I2C-Bibliothek einbinden
#include "RTClib.h"  // RTC-Bibliothek einbinden
RTC_DS1307 RTC;      // RTC Modul

float get_ampere(int x){
  //par_1=messure pin, output value in amp float
  const unsigned long sampleTime = 100000UL;                           // sample over 100ms, it is an exact number of cycles for both 50Hz and 60Hz mains
  const unsigned long numSamples = 250UL;                               // choose the number of samples to divide sampleTime exactly, but low enough for the ADC to keep up
  const unsigned long sampleInterval = sampleTime/numSamples;  // the sampling interval, must be longer than then ADC conversion time
  const int adc_zero = 510; // relative digital zero of the arudino input from ACS712 (could make this a variable and auto-adjust it) 
  const int currentPin = x;
  unsigned long currentAcc = 0;
  unsigned int count = 0;
  unsigned long prevMicros = micros() - sampleInterval ;
  float rms = 0; 
  pinMode(currentPin, INPUT); 
  while (count < numSamples){
    if (micros() - prevMicros >= sampleInterval){
      long adc_raw = analogRead(currentPin) - adc_zero;
      currentAcc += (unsigned long)(adc_raw * adc_raw);
      ++count;
      prevMicros += sampleInterval;
    }
  }
  rms = sqrt((float)currentAcc/(float)numSamples) * (75.7576 * 0.3297 / 1024.0 );
  return rms;
}
void set_relais(int x, int y){
  //offset pins +4 (start pin 5)
  //par_1 = relais no, par_2 = set 1/0 or switch;
  int currentPin = constrain(x+4, 5, 12);
  int invert = 1;
  int test = 0;
  pinMode(currentPin, OUTPUT);
  digitalWrite(currentPin, LOW);
  if (y == 1) {
    digitalWrite(currentPin, LOW);
    EEPROM.write(currentPin-4,0);
  }
  if (y == 0) {
    digitalWrite(currentPin, HIGH);
    EEPROM.write(currentPin-4,1);
  }
   if (y > 1) {
    invert = EEPROM.read(currentPin-4);
    invert = !invert;
    digitalWrite(currentPin, invert);
    EEPROM.write(currentPin-4,invert);
  }
}
void load_relais() {
  byte pin = 5;
  byte state = 0;
  while (pin <= 12) {
    state = EEPROM.read(pin-4);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state);
    pin = pin+1;
  }
}
void save_relais() {
  byte pin = 5;
  byte state = 0;
  while (pin <= 12) {
    state = digitalRead(pin);
    EEPROM.write(pin-4, state);
    pin = pin+1;
  }
}
void s_print_ampere(float x) {
  float store = x;
  if (store > 0.18) {
    Serial.print("Ampere: ");
    Serial.print(store);
    Serial.println();
    store = store * 225;
    Serial.print("Watt: ");
    Serial.print(store);
    Serial.println();
  }
  else {
    Serial.println("off");
  }
}
void s_print_time() {
        DateTime now=RTC.now(); // aktuelle Zeit abrufen
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println();
}
void check_eeprom(int x) {
  int address = 0;
  byte value;
  while (address <= x) {
    value = EEPROM.read(address);  
    Serial.print(address);
    Serial.print(" ");
    Serial.print(value, DEC);
    Serial.println();
    // advance to the next address of the EEPROM
    address = address + 1; 
    // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
    // on address 512, wrap around to address 0
  }
}

void setup()
{
 Serial.begin(9600);
 Serial.println("init 1.4");
 Wire.begin();
 RTC.begin();
 if (! RTC.isrunning()) {
   RTC.adjust(DateTime(__DATE__, __TIME__));
   Serial.println("Echtzeituhr gestartet Systemzeit gesetzt.");
 }
 load_relais();
}

int test = 1;

void loop()
{ 
  
  while (Serial.available() > 0) {

    int input1 = Serial.parseInt();
    int input2 = Serial.parseInt();
    int input3 = Serial.parseInt();
    if (Serial.read() == '\n') {

      if (input1 == 1) {
        float ampere = get_ampere(3);
        s_print_ampere(ampere);
      }
      if (input1 == 2) {
        set_relais(input2,input3);
      }
      if (input1 == 3) {
        save_relais();
      }
      if (input1 == 4) {

      }
      if (input1 == 5) {
        while (Serial.available() <= 0) {
          Serial.print("              ");
          s_print_time();
          int loop = 0;
          while (loop <= 8) {
            Serial.println();
            loop = loop + 1;
          }
          delay(1000);
        }

      }
      //debug
      if (input1 == 0) {
        Serial.println("---------------");
        s_print_time();
        Serial.print("input:");
        Serial.print(" ");
        Serial.print(input1);
        Serial.print(" ");
        Serial.print(input2);
        Serial.print(" ");
        Serial.println(input3);
        while (test <=8) {
          Serial.print("Status_pin_");
          Serial.print(test);
          Serial.print(": ");
          Serial.println(digitalRead(test+4)); 
          test = test+1;
        }
        test = 1; 
        check_eeprom(input2);
        Serial.println("---------------");
      }
    }
  }
  
}








