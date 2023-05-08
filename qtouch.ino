#include <CircularBuffer.h>

// This code works only on SAMD21

//Use interrupt to optimize analogRead
#include "Adafruit_ZeroTimer.h"
int COMPARE_P = 48000;
int COMPARE_QT = 48000/2;

#include "qtouch.h"
#include "distance.h"
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
    {62, 0 }     // Note Number 60 on MIDI channel 1
  },
  NoteQtouch {
    QT2,            // QT2 is also pin A1 of SAMD21
    {64, 0 }     // Note Number 60 on MIDI channel 1
  },
  NoteQtouch {
    QT3,            // QT3 is also pin A6 of SAMD21
    {65, 0 }     // Note Number 60 on MIDI channel 1
  },
  NoteQtouch {
    QT4,            // QT4 is also pin A7 of SAMD21
    {69, 0 }     // Note Number 60 on MIDI channel 1
  },
  NoteQtouch {
    QT5,            // QT5 is also pin A8 of SAMD21
    {67, 0 }     // Note Number 60 on MIDI channel 1
  },
  NoteQtouch {
    QT6,            // QT6 is also pin A9 of SAMD21
    {60, 0 }     // Note Number 60 on MIDI channel 1
  },
  NoteQtouch{
    QT7,            // QT7 is also pin A10 of SAMD21
    {71, 0 }     // Note Number 60 on MIDI channel 1
  }
};

// piezo disk connected on analog A3
piezo Piezo {
  A3,            // Analog pin
  {48, 0 }      // Note Number 48 on MIDI channel 1
  //  3
 };
//global variable for piezo
uint8_t qtouchActif = 0;

// timer to prioritize piezo
Adafruit_ZeroTimer zerotimerP = Adafruit_ZeroTimer(3);
void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}

// distance sensor uses Adafruit_VL53L0X library
distancePB Distance(0); // filter amount
long distanceTimer;
long bufferTimer;

// flash storage to stock the activated qtouch zone
//FlashStorage(my_flash_store, uint8_t);
//bool Tab_state[7] = {0,0,0,0,0,0,0};
CircularBuffer<uint8_t, 7> QTbuffer;
CircularBuffer<uint8_t, 7> tempBuffer;
void setup() {
  distanceTimer = millis();
  Serial.begin(115200);
  qTouchBegin();
  timerPBegin();
//  Distance.begin();

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

void printQTbuffer() {
	if (QTbuffer.isEmpty()) {
		Serial.println("empty");
	} else {
		Serial.print("[");
		for (decltype(QTbuffer)::index_t i = 0; i < QTbuffer.size() - 1; i++) {
			Serial.print(QTbuffer[i]);
			Serial.print(",");
		}
		Serial.print(QTbuffer[QTbuffer.size() - 1]);
		Serial.print("] (");

		Serial.print(QTbuffer.size());
		Serial.print("/");
		Serial.print(QTbuffer.size() + QTbuffer.available());
		if (QTbuffer.isFull()) {
			Serial.print(" full");
		}

		Serial.println(")");
	}
}

void loop() {
    qTouchLoop();

//  if ((millis() - distanceTimer > 100) && Piezo.state == 0) {
  // if ((millis() - distanceTimer > 100)) {
  //   distanceTimer = millis();
  //   Distance.update();
  // }
  // if (millis() - bufferTimer > 1000) {
  //   bufferTimer = millis();
  //   printQTbuffer();
  // }
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
  for (int i = 0; i < 7; i ++){
  tableauQtouch[i].calibrate();   
  }
//    Distance.begin();
  }
 // delay(20);
}



void qTouchBegin() {
  for (int i = 0; i < 7; i ++){
  tableauQtouch[i].begin();   
  }
}
int bufferSearch(uint8_t note) {
  for (int i = 0; i < 7; i ++) {
    int buff = QTbuffer[i];
    if (QTbuffer[i] == note) return i;
  }
  return -1;
}
void removeFromBuffer(uint8_t pos) {

  Serial.print("removing ");Serial.println(QTbuffer[pos]);
  for (uint8_t i = 0; i > QTbuffer.size() ; i ++) {
    if (i != pos) tempBuffer.unshift(QTbuffer.pop());
  }
  QTbuffer.clear();
  for (uint8_t i = 0; i > tempBuffer.size() ; i ++) {
    QTbuffer.unshift(tempBuffer.pop());
  }
}
void qTouchLoop() {
  for (uint8_t i = 0; i < 7; i ++){
    tableauQtouch[i].update(); 
    uint8_t note = tableauQtouch[i].note;
    if (tableauQtouch[i].getState() == 1) {
      int pos = bufferSearch(note);

//      Serial.print("pos ");Serial.println(pos);
      if (pos < 0) {
        QTbuffer.push(note);
//      Serial.print("Buffer Push ");Serial.println(note);
      }
    }
    else {
      if (QTbuffer.isEmpty()) {
// sometimes buffer is empty before sending note off??
      }
      else if (bufferSearch(note) >= 0) {
        removeFromBuffer(note);
        tableauQtouch[i].sendAfterTouch(0);
//        Piezo.noteOff(note);
      }
    }
  }
}

// Piezo's timer callback function

void TimerCallback0(){
//    qTouchLoop(); 
  // if (millis() - distanceTimer > 100) {
  //   distanceTimer = millis();
  //   Distance.update();
  // } 
  	if (QTbuffer.isEmpty()) {
      
//		      Piezo.update((uint8_t)(0));
	} else {
      Piezo.update((uint8_t)(QTbuffer.last()));
        // if(DoneSendNote[i] == 1){
          //midiEventPacket_t event = {0x09, 0x90 | 0, 60+i, 65};
          //MidiUSB.sendMIDI(event);
          // tableauQtouch[i].IgnoreNote = 1;
          // DoneSendNote[i] = 0;
//        }
    }
    // else
    //   Nqt = Nqt+1;
 // }
  // if (Nqt == 7){
  //     Piezo.update((uint8_t)(0));
  // }

}

// timer setup

void timerPBegin(){ 
  tc_clock_prescaler prescaler = TC_CLOCK_PRESCALER_DIV1;
  zerotimerP.enable(false);
  zerotimerP.configure(prescaler,       // prescaler
          TC_COUNTER_SIZE_16BIT,       // bit width of timer/counter
          TC_WAVE_GENERATION_MATCH_PWM // frequency or PWM mode
          );
  zerotimerP.setCompare(0, COMPARE_P);
  zerotimerP.setCallback(true, TC_CALLBACK_CC_CHANNEL0, TimerCallback0);
  zerotimerP.enable(true);
}

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
