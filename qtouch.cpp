#include "qtouch.h"

CCQtouch::CCQtouch(int pin, MIDIAddress address) {
  qt = Adafruit_FreeTouch(pin, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
  _address = address;
}

void CCQtouch::begin() {
  qt.begin();
  qt_floor = qt.measure();
  //IgnoreNote = 0;
  //calibrate();
};

void CCQtouch::calibrate(){
  qt_floor = qt.measure();
};

void CCQtouch::loop() {
  //int qt_measure = (( N * qt_measure ) + qt.measure() ) / ( N + 1 );
  int qt_measure = qt.measure();
  // set roundoff at instantiation
  int roundOff = 5;
  int range = 1014 - qt_floor + roundOff;
  CCvalue = 127 * (qt_measure - qt_floor + roundOff) / range;
  if(qt_measure > qt_floor + roundOff) {
    sendController();
  };
};

void CCQtouch::sendController() {
      midiEventPacket_t event = {0x0B, 0xB0 | _address.channel, _address.address, CCvalue};
      MidiUSB.sendMIDI(event);
};

NoteQtouch::NoteQtouch(int pin, MIDIAddress address) {
  qt = Adafruit_FreeTouch(pin, OVERSAMPLE_64, RESISTOR_50K, FREQ_MODE_HOP);
  _address = address;
  _pin = pin; 
  //Hysteresis <uint8_t> hysteresis(10);
};

void NoteQtouch::begin() {
  qt.begin();
  qt_floor = qt.measure();
//  hysteresis.set(10);
  //calibrate();
};

void NoteQtouch::calibrate(){
  qt_floor = qt.measure();
};

void NoteQtouch::update() {
  //int qt_measure = (( N * qt_measure ) + qt.measure() ) / ( N + 1 );
  int qt_measure = qt.measure();
  int roundOff = 15;
  int range = 1014 - qt_floor + roundOff;
  velocity = 127 * (qt_measure - qt_floor + roundOff) / range;
  if((qt_measure > qt_floor + roundOff) && qt_memory == 0) {
    qt_memory = qt_measure;
    //sendNoteOn();
    Serial.print("Note On"); Serial.println(_pin);
    IgnoreNote = 0;
    setState(1);
  };
  if((qt_measure < (qt_floor + roundOff)) && qt_memory != 0) {
    qt_memory = 0;    
    //sendNoteOff();  
    setState(0);
    sendAfterTouch();
  }
  if((qt_memory > 0) && (qt_memory != qt_measure)) {
      qt_memory = qt_measure;     
      sendAfterTouch();
    }
};

void NoteQtouch::setState(bool etat) {
     state = etat;
};

bool NoteQtouch::getState() {
     return state;
};


void NoteQtouch::sendNoteOn(int velo) {
      velopiezo = (uint8_t)(velo);
      //midiEventPacket_t event = {0x09, 0x90 | _address.channel, _address.address, velocity};
      midiEventPacket_t event = {0x09, 0x90 | _address.channel, _address.address, velopiezo};
      MidiUSB.sendMIDI(event);
};

void NoteQtouch::sendNoteOff() {
      midiEventPacket_t event = {0x08, 0x80 | _address.channel, _address.address, 0};
      MidiUSB.sendMIDI(event);
};

void NoteQtouch::sendAfterTouch() {
      uint8_t afterTouch = velocity * 3;
      if (afterTouch > 127) afterTouch = 127;
      midiEventPacket_t event = {0x0A, 0xA0 | _address.channel, _address.address, afterTouch};
      MidiUSB.sendMIDI(event);
};
