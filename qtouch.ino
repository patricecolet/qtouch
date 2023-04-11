// This code works only on SAMD21

//#include "piezo.h"
#include "qtouch.h"
//#include "distance.h"
#include "piezo.hpp"

//#include <Control_Surface.h>

//USBMIDI_Interface midi;

//const int buttonPin = 2;     // the number of the pushbutton pin

// SAMD21 Doesn't support pin PULL_UP mode,
// so we use a resistor with one end connected to 3.3v
// and the other end connected to Digital input and switch output.
// Switch input is connected to ground.

// variables will change:
//int buttonState = HIGH;         // variable for reading the pushbutton status

NoteQtouch tableauQtouch[] = {

NoteQtouch {
  QT1,            // QT1 is also pin A0 of SAMD21
  {60, 0 }     // Note Number 60 on MIDI channel 1
},

NoteQtouch {
  QT2,            // QT2 is also pin A1 of SAMD21
  {61, 0 }     // Note Number 60 on MIDI channel 1
},

NoteQtouch {
  QT3,            // QT3 is also pin A6 of SAMD21
  {62, 0 }     // Note Number 60 on MIDI channel 1
},

NoteQtouch {
  QT4,            // QT4 is also pin A7 of SAMD21
  {63, 0 }     // Note Number 60 on MIDI channel 1
},

NoteQtouch {
  QT5,            // QT5 is also pin A8 of SAMD21
  {64, 0 }     // Note Number 60 on MIDI channel 1
},

NoteQtouch {
  QT6,            // QT6 is also pin A9 of SAMD21
  {65, 0 }     // Note Number 60 on MIDI channel 1
},

NoteQtouch{
  QT7,            // QT7 is also pin A10 of SAMD21
  {66, 0 }     // Note Number 60 on MIDI channel 1
}
};

//unsigned long piezoTimer;
//int prevpiezoRead;
piezo Piezo {
  A3,            // Analog pin
  {48, 0 }     // Note Number 48 on MIDI channel 1
};
//
//  distancePB Distance(0); // MIDI channel 1 is '0'
void setup() {
  disableALL();

  Serial.begin(115200);
    qTouchBegin();
  //  initPiezo();
  // wait until serial port opens for native USB devices
  //  while (! Serial) {
  //    delay(1);
  //  }
  //  if (!Distance.begin()) {
  //    Serial.println(F("Failed to boot VL53L0X"));
  //    while(1);
  //  }

  //pinMode(buttonPin, INPUT);
}

void loop() {
  //  VL53L0X_RangingMeasurementData_t measure;
  //  Distance.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  //  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
  //    if (measure.RangeMilliMeter <= 800){
  //     Serial.println(measure.RangeMilliMeter);
  //     sendPitchbend(measure.RangeMilliMeter);
  //    }
  //  }
  qTouchLoop();
 // qTouchGetState();
  if (tableauQtouch[0].getState() == 1) 
    Serial.println(tableauQtouch[0].getState());

  Piezo.update();

  midiEventPacket_t midirx;
  // read the midi note
  midirx = MidiUSB.read();
  char midiheader = midirx.header;
  char midibyte1 = midirx.byte1;
  char midinote =  midirx.byte2;
  char midivelocity =  midirx.byte3;

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  // if (buttonState == LOW || (midinote == 0x3c)) {
  if (midinote == 0x3c) {
    Serial.println("calibrate");
    qTouchBegin();
  }
 // delay(20);
}

void qTouchBegin() {
  for (int i = 0; i < 7; i ++){
  tableauQtouch[i].begin();   
  }
}

void qTouchLoop() {
  for (int i = 0; i < 7; i ++){
  tableauQtouch[i].loop();   
  }
}

/*
void qTouchGetState(){
  
  Serial.print('note60'); Serial.println(note60.getState());
  Serial.print('note61'); Serial.println(note61.getState());
  Serial.print('note62'); Serial.println(note62.getState());
  Serial.print('note63'); Serial.println(note63.getState());
  Serial.print('note64'); Serial.println(note64.getState());
  Serial.print('note65'); Serial.println(note65.getState());
  Serial.print('note66'); Serial.println(note66.getState());

  bool state60 = note60.getState();
  bool state61 = note61.getState();
  bool state62 = note62.getState();
  bool state63 = note63.getState();
  bool state64 = note64.getState();
  bool state65 = note65.getState();
  bool state66 = note66.getState();
}
*/

void disableALL() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}
//void sendPitchbend(int value) {
//  byte lowValue = value & 0x7F;
//  byte highValue = value >> 7;
//  midiEventPacket_t event ={0x0E, 0xE0 | 0, lowValue, highValue};
//  MidiUSB.sendMIDI(event);
//}
