#include "piezo.hpp"

piezo::piezo(pin_t pin, MIDIAddress address) {
    _address = address;
    _pin = pin;  
};

void piezo::update() {
    int piezoRead = analogRead(_pin);
    piezoState<int> Piezo;
  // switch case to update piezo.state 
  switch(Piezo.state) {
    case UNDERTHRESHOLD:
      if(piezoRead > Piezo.threshold && piezoRead > prevpiezoRead) {
        Piezo.state = SIGNAL;
          Serial.println("\n*************************************");
          Serial.print("PIEZO: "); Serial.println(Piezo.state); 
      }
      break;
    case SIGNAL: 
              Serial.println("\n*************************************");
          Serial.print("PIEZO_SIGNAL: "); Serial.println(Piezo.state);   
      if (piezoRead > prevpiezoRead)
        Piezo.state = RISING;
                  Serial.println("\n*************************************");
          Serial.print("PIEZO_SIGNAL: "); Serial.println(Piezo.state);
      break;
      
    case RISING: 
                      Serial.println("\n*************************************");
          Serial.print("PIEZO_RISING: "); Serial.println(Piezo.state);     
      if (piezoRead < prevpiezoRead)
        Piezo.state = PEAK;
      break;
    case PEAK:
                          Serial.println("\n*************************************");
          Serial.print("PIEZO_RISING: "); Serial.println(Piezo.state);
      if (Piezo.prevstate == PEAK)
        Piezo.state = FALLING;
      break;
    case FALLING:
                          Serial.println("\n*************************************");
          Serial.print("PIEZO_RISING: "); Serial.println(Piezo.state);
      if (piezoRead > prevpiezoRead && (millis()- piezoTimer)> Piezo.debounceTime)
        Piezo.state = RISING;
      if (piezoRead < Piezo.threshold)
        Piezo.state = UNDERTHRESHOLD;
      break;
  }
//  Serial.println("\n*************************************");
//  Serial.print("PIEZO: "); Serial.println(piezoRead);
//  Serial.print("PREVIOUS PIEZO: "); Serial.println(prevpiezoRead);
//  Serial.print("PIEZO STATE: "); Serial.println(Piezo.state);

  // switch case for actions in each piezo.state 
//  switch(Piezo.state) {
//    case UNDERTHRESHOLD:
//      break;
//    case SIGNAL:
//      piezoTimer = millis();
//      playnote(piezoRead);
//      break;
//    case RISING:
//      playnote(piezoRead);
//      break;
//    case PEAK:
//      Serial.println("\n*************************************");
//  Serial.print("PIEZO: "); Serial.println("PEAK");
//      piezoNote();
//      Piezo.peak = 0;
//      break;
//    case FALLING:
//      playnote(piezoRead);
//      break;
//  }
  
  // save prevoius values in memory variables
  prevpiezoRead = piezoRead;
  Piezo.prevstate = Piezo.state;
  
}


void piezo::playnote(int piezoRead) {
  piezoState<int> Piezo;
  velocity = 127 * piezoRead / (Piezo.sensitivity - Piezo.threshold);
  if (velocity > 127) velocity = 127;
  if (velocity > Piezo.peak) Piezo.peak = velocity;
}


void piezo::piezoNote() {
  Serial.println("\n*************************************");
  Serial.print("PIEZO: "); Serial.println(velocity);  
      midiEventPacket_t noteOn = {0x09, 0x90 | _address.channel, _address.address, velocity};
      MidiUSB.sendMIDI(noteOn);
      midiEventPacket_t noteOff = {0x08, 0x80 | _address.channel, _address.address, 0};
      MidiUSB.sendMIDI(noteOff);
};