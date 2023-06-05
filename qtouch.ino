#include <CircularBuffer.h>

// This code works only on SAMD21

//Use interrupt to optimize analogRead
#include "Adafruit_ZeroTimer.h"
int COMPARE_P = 48000;
int COMPARE_QT = 48000/2;

/*
#include "distance.h"
const byte VL53LOX_ShutdownPin = 2;
// distance sensor uses Adafruit_VL53L0X library
distancePB Distance(5); // filter amount
long distanceTimer;
long bufferTimer;
*/

#include "qtouch.h"
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

CircularBuffer<uint8_t, 7> QTbuffer;
CircularBuffer<uint8_t, 7> tempBuffer;

int bufferSearch(uint8_t note) {
  if (QTbuffer.isEmpty()) return -1; 
  for (int i = 0; i < 7; i ++) {
    int buff = QTbuffer[i];
    if (QTbuffer[i] == note) return i;
  }
  return -1;
}

void removeFromBuffer(uint8_t pos) {

//  Serial.print("removing ");Serial.println(QTbuffer[pos]);
  for (uint8_t i = 0; i > QTbuffer.size() ; i ++) {
    if (i != pos) tempBuffer.unshift(QTbuffer.pop());
  }
  QTbuffer.clear();
  for (uint8_t i = 0; i > tempBuffer.size() ; i ++) {
    QTbuffer.unshift(tempBuffer.pop());
  }
}

#include "piezo.hpp"
// piezo disk connected on analog A3
piezo Piezo {
  A3,            // Analog pin
  {48, 0 }      // Note Number 48 on MIDI channel 1
  //  3
 };

void qTouchBegin() {
  for (int i = 0; i < 7; i ++){
  tableauQtouch[i].begin();   
  }
}



void setup() {
  Serial.begin(115200);

  Serial.println(F("HEXAPAD SETUP"));
  delay(1000);
/*
  distanceTimer = millis();
  pinMode(VL53LOX_ShutdownPin, OUTPUT);
  if (!Distance.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    Serial.println("Adafruit VL53L0X XShut set Low to Force HW Reset");
    digitalWrite(VL53LOX_ShutdownPin, LOW);
    delay(100);
    digitalWrite(VL53LOX_ShutdownPin, HIGH);
    Serial.println("Adafruit VL53L0X XShut set high to Allow Boot");
    delay(100);
  }
  digitalWrite(VL53LOX_ShutdownPin, LOW);
  */
  qTouchBegin();
  delay(1000);
    for (int i = 0; i < 7; i ++){
//  tableauQtouch[i].begin();  
  tableauQtouch[i].calibrate();   
  }
  timerPBegin();
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

void calibrate() {
    midiEventPacket_t midirx;
  // read the midi note
  midirx = MidiUSB.read();
  char midiheader = midirx.header;
  char midibyte1 = midirx.byte1;
  char midinote =  midirx.byte2;
  char midivelocity =  midirx.byte3;

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  // if (buttonState == LOW || (midinote == 0x3c)) {

//calibrate on note off
  if (midinote == 0x3c && midivelocity == 0) {
    Serial.println("calibrate");
  for (int i = 0; i < 7; i ++){
//  tableauQtouch[i].begin(); 
  tableauQtouch[i].calibrate();   
  }
//    Distance.begin();
  }
}

void qTouchLoop() {
  for (uint8_t i = 0; i < 7; i ++) {
    tableauQtouch[i].update(); 
    uint8_t note = tableauQtouch[i].note;
    if (tableauQtouch[i].getState() == 1) {
      int pos = bufferSearch(note);
      if (pos < 0) {
        QTbuffer.push(note);
      }
    }
    else 
      {
        if (bufferSearch(note) >= 0) {
        removeFromBuffer(note);
        // Sometimes buffer is bouncing
//        Piezo.noteOff(note);
//        tableauQtouch[i].sendAfterTouch(0);
      }
    }
  }
}

void loop() {
  calibrate();
  qTouchLoop();
//   if ((millis() - distanceTimer > 100) && Piezo.state == 0) {
// //    if ((millis() - distanceTimer > 100)) {
//       distanceTimer = millis();
//   digitalWrite(VL53LOX_ShutdownPin, HIGH);
//       if (QTbuffer.isEmpty()) 
//       Distance.update();

//   digitalWrite(VL53LOX_ShutdownPin, LOW);
// //    }
//   }
}
// timer to prioritize piezo
Adafruit_ZeroTimer zerotimerP = Adafruit_ZeroTimer(3);
void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}


// Piezo's timer callback function

void TimerCallback0(){
  	if (QTbuffer.isEmpty()) {
  //   if ((millis() - distanceTimer > 100)) {
  //     distanceTimer = millis();
  // digitalWrite(VL53LOX_ShutdownPin, HIGH); 
  //     Distance.update();

  // digitalWrite(VL53LOX_ShutdownPin, LOW);
  //   }
//		      Piezo.update((uint8_t)(48));
	} else {
      Piezo.update((uint8_t)(QTbuffer.last()));
    }
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
