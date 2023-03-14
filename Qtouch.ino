// This code works only on SAMD21
#include "Qtouch.h"
//#include <Control_Surface.h>

//USBMIDI_Interface midi;

const int buttonPin = 2;     // the number of the pushbutton pin

// SAMD21 Doesn't support pin PULL_UP mode,
// so we use a resistor with one end connected to 3.3v
// and the other end connected to Digital input and switch output.
// Switch input is connected to ground.

// variables will change:
int buttonState = HIGH;         // variable for reading the pushbutton status

NoteQtouch note60 {
  QT1,            // QT1 is also pin A0 of SAMD21
   {60,0 }      // Note Number 60 on MIDI channel 1
    };

NoteQtouch note61 {
  QT2,            // QT2 is also pin A1 of SAMD21
   {61,0 }      // Note Number 60 on MIDI channel 1
    };

NoteQtouch note62 {
  QT3,            // QT3 is also pin A6 of SAMD21
   {62,0 }      // Note Number 60 on MIDI channel 1
    };

NoteQtouch note63 {
  QT4,            // QT4 is also pin A7 of SAMD21
   {63,0 }      // Note Number 60 on MIDI channel 1
    };

NoteQtouch note64 {
  QT5,            // QT5 is also pin A8 of SAMD21
   {64,0 }      // Note Number 60 on MIDI channel 1
    };

NoteQtouch note65 {
  QT6,            // QT6 is also pin A9 of SAMD21
   {65,0 }      // Note Number 60 on MIDI channel 1
    };

NoteQtouch note66 {
  QT7,            // QT7 is also pin A10 of SAMD21
   {66,0 }      // Note Number 60 on MIDI channel 1
    };

void setup() {
  note60.begin(); // calibrate Qtouch for note 60
  note61.begin(); // calibrate Qtouch for note 61
  note62.begin(); // calibrate Qtouch for note 62
  note63.begin(); // calibrate Qtouch for note 63
  note64.begin(); // calibrate Qtouch for note 64
  note65.begin(); // calibrate Qtouch for note 65
  note66.begin(); // calibrate Qtouch for note 66
  pinMode(buttonPin, INPUT);
}

void loop() {
  note60.loop(); // update note 60 sensor - pad du milieu
  note61.loop(); // update note 60 sensor - elui par lequel le fil du 60 passe
  note62.loop(); // update note 60 sensor
  note63.loop(); // update note 60 sensor
  note64.loop(); // update note 60 sensor
  note65.loop(); // update note 60 sensor
  note66.loop(); // update note 60 sensor
  
  midiEventPacket_t midirx;
  // read the midi note 
  midirx = MidiUSB.read();
  char midiheader = midirx.header;
  char midibyte1 = midirx.byte1;
  char midinote =  midirx.byte2;
  char midivelocity =  midirx.byte3;

 
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == LOW || (midinote == 0x3c)) {
        Serial.println("calibrate");
  note60.begin(); // calibrate Qtouch for note 60
  note61.begin(); // calibrate Qtouch for note 61
  note62.begin(); // calibrate Qtouch for note 62
  note63.begin(); // calibrate Qtouch for note 63
  note64.begin(); // calibrate Qtouch for note 64
  note65.begin(); // calibrate Qtouch for note 65
  note66.begin(); // calibrate Qtouch for note 66
  } 
}
