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
      if (piezoRead > prevpiezoRead && (millis()- piezoTimer)> Piezo.debounceTime)
        Piezo.state = RISING;
      if (piezoRead < Piezo.threshold)
        Piezo.state = UNDERTHRESHOLD;
      break;
  }
  Serial.println("\n*************************************");
  Serial.print("PIEZO: "); Serial.println(piezoRead);
  Serial.print("PREVIOUS PIEZO: "); Serial.println(prevpiezoRead);
  Serial.print("PIEZO STATE: "); Serial.println(Piezo.state);

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
      piezoNote();
      Piezo.peak = 0;
      break;
    case FALLING:
      playnote(piezoRead);
      break;
  }
  
  // save prevoius values in memory variables
  prevpiezoRead = piezoRead;
  Piezo.prevstate = Piezo.state;
  
}
