

void setup()
{
 Serial.begin(9600);
 Serial.println("init");
}

void loop()
{ 
  

    while (Serial.available()) {
      int ch = Serial.read()-48;
      if (ch == 1) {
        float ampere = get_ampere(3);
        s_print_ampere(ampere);
      }
      //memset(intBuffer, 0, sizeof(intBuffer)); //clear array
      if (ch == 2) {
        set_relais(1,1);
      }
      if (ch == 3) {
        set_relais(1,0);
      }
    }
}



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

void set_relais(int x, int y){
  //offset pins +4 (start pin 5)
  //par_1 = relais no, par_2 = set 1/0;
  int currentPin = x+4;
  pinMode(currentPin, OUTPUT);
  digitalWrite(currentPin, LOW);
  if (y == 1) {
    digitalWrite(currentPin, LOW);
  }
  else {
    digitalWrite(currentPin, HIGH);
  }
}

