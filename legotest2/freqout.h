#ifndef PC_FREQOUT_H
#define PC_FREQOUT_H

#include "Arduino.h"// "WProgram.h"

void freqout(int outpin, long freq, int t)  // freq in hz, t in ms
{
  long hperiod;                               
  long cycles, i;
  pinMode(outpin, OUTPUT);  

  hperiod = (500000 / freq) - 7; 

  cycles = ((long)freq * (long)t) / 1000;    

  for (i=0; i<= cycles; i++){               
    digitalWrite(outpin, HIGH); 
    delayMicroseconds(hperiod);
    digitalWrite(outpin, LOW); 
    delayMicroseconds(hperiod - 1);     
  }

  pinMode(outpin, INPUT);               

}

int readIRDetector(int LEDPin, int detectorPin) {
    freqout(LEDPin,38250, 1);
    return !digitalRead(detectorPin);
}

#endif // PC_FREQOUT_H

