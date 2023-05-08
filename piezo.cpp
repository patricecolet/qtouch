#include "piezo.hpp"
bool sendNote = 0;

piezo::piezo(pin_t pin, MIDIAddress address) {
    _address = address;
    _pin = pin;  
};
void piezo::update(uint8_t memoNote) {
  int piezoRead = analogRead(_pin);
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
      if (sendNote == 1)
        Piezo.state = SENDNOTE;
      break;
    case SENDNOTE:
      if (Piezo.prevstate == SENDNOTE)
        Piezo.state = FALLING;
      break;
    case FALLING:  
      if (piezoRead < Piezo.threshold && (millis()- piezoTimer)> Piezo.debounceTime)
        Piezo.state = UNDERTHRESHOLD;
      break;
  }

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
        sendNote = 1;
      break;
    case SENDNOTE:
      if(memoNote == 0) piezoNote(_address.address);
      else piezoNote(memoNote);
      sendNote = 0;
      Piezo.peak = 0;
      break;
    case FALLING:
      break;
  }
  prevpiezoRead = piezoRead;
  Piezo.prevstate = Piezo.state;
  state = Piezo.state;
}

void piezo::playnote(int piezoRead) {
  velocity = 127 * piezoRead / (Piezo.sensitivity - Piezo.threshold);
  if (velocity > 127) velocity = 127;
  if (velocity > Piezo.peak) Piezo.peak = velocity;
}

// Send note midi
void piezo::piezoNote(uint8_t note) {  
//  if (note != 48){
  midiEventPacket_t noteOn = {0x09, 0x90 | _address.channel, note, Piezo.peak};
  MidiUSB.sendMIDI(noteOn);
  midiEventPacket_t noteOff = {0x08, 0x80 | _address.channel, note, 0};
  MidiUSB.sendMIDI(noteOff);
//  }
};
void piezo::noteOff(uint8_t note) {  
//  if (note != 48){
  // midiEventPacket_t noteOn = {0x09, 0x90 | _address.channel, note, Piezo.peak};
  // MidiUSB.sendMIDI(noteOn);
  midiEventPacket_t noteOff = {0x08, 0x80 | _address.channel, note, 0};
  MidiUSB.sendMIDI(noteOff);
//  }
};
