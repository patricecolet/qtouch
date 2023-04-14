#include "piezo.hpp"
//global variable for piezo's velocity
//uint8_t velopiezo = 0;

piezo::piezo(pin_t pin, MIDIAddress address) {
     //zerotimer = Adafruit_ZeroTimer(timer);
    _address = address;
    _pin = pin;  
    //SendVelo = 0;
};

void piezo::update(uint8_t memoNote) {
  int piezoRead = analogRead(_pin);
  //int piezoRead = piezoread;
  //Serial.print("TIMER: "); Serial.println(piezoRead);
  // switch case to update piezo.state
  switch(Piezo.state) {
    case UNDERTHRESHOLD:
      if(piezoRead > Piezo.threshold && piezoRead > prevpiezoRead)
        Piezo.state = SIGNAL;
      break;
    case SIGNAL:   
      if (piezoRead > prevpiezoRead)
        Piezo.state = RISING;
      break;
    case RISING:    
      if (piezoRead < prevpiezoRead)
        Piezo.state = PEAK;
      break;
    case PEAK:
      if (Piezo.prevstate == PEAK)
        Piezo.state = FALLING;
      break;
    case FALLING:  
      if (piezoRead < Piezo.threshold && (millis()- piezoTimer)> Piezo.debounceTime)
        Piezo.state = UNDERTHRESHOLD;
      break;
  }
//  Serial.println("\n*************************************");
//  Serial.print("PIEZO: "); Serial.println(piezoRead);
//  Serial.print("PREVIOUS PIEZO: "); Serial.println(prevpiezoRead);
//  Serial.print("PIEZO STATE: "); Serial.println(Piezo.state);
// delay(1);

  // switch case for actions in each piezo.state 
  switch(Piezo.state) {
    case UNDERTHRESHOLD:
      break;
    case SIGNAL:
      piezoTimer = millis();
      playnote(piezoRead);
      break;
    case RISING:
      playnote(piezoRead);
      break;
    case PEAK:
      //velopiezo = Piezo.peak;
      //Serial.print("Velocity piezo SWITCH : "); Serial.println(Send);
      //SendVelo = Piezo.peak;
      if(memoNote == 0)
        piezoNote(_address.address);
      else
        piezoNote(memoNote);
      Piezo.peak = 0;
      break;
    case FALLING:
      //velopiezo = 0;
      //SendVelo = 0;
      break;
  }
  
  // save prevoius values in memory variables
  prevpiezoRead = piezoRead;
  Piezo.prevstate = Piezo.state;
  //return SendVelo;
}

void piezo::playnote(int piezoRead) {
  velocity = 127 * piezoRead / (Piezo.sensitivity - Piezo.threshold);
  if (velocity > 127) velocity = 127;
  if (velocity > Piezo.peak) Piezo.peak = velocity;
}

void piezo::piezoNote(uint8_t note) {   
  midiEventPacket_t noteOn = {0x09, 0x90 | _address.channel, note, Piezo.peak};
  MidiUSB.sendMIDI(noteOn);
  midiEventPacket_t noteOff = {0x08, 0x80 | _address.channel, note, 0};
  MidiUSB.sendMIDI(noteOff);
};
