// This code works only on SAMD21

//Use interrupt to optimize analogRead
#include "Adafruit_ZeroTimer.h"
int COMPARE_P = 48000;
int COMPARE_QT = 48000/2;

//#include "piezo.h"
#include "qtouch.h"
#include "distance.h"
#include "piezo.hpp"
#include <FlashStorage.h>

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
distancePB Distance(0); // MIDI channel 1 is '0'
long distanceTimer;

// flash storage to stock the activated qtouch zone
FlashStorage(my_flash_store, uint8_t);
bool Tab_state[7] = {0,0,0,0,0,0,0};

void setup() {
  distanceTimer = millis();
  Serial.begin(115200);
  qTouchBegin();
  timerPBegin();
  Distance.begin();
  //my_flash_store.write(qtouchActif);

//   wait until serial port opens for native USB devices
//    while (! Serial) {
//      delay(1);
//    }
//    if (!Distance.begin()) {
//      Serial.println(F("Failed to boot VL53L0X"));
//      while(1);
//    }
//pinMode(buttonPin, INPUT);
}

void loop() {
  if (recheckQt == 0)
    qTouchLoop();

  if (millis() - distanceTimer > 100) {
    distanceTimer = millis();
    Distance.update();
  }

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
    Distance.begin();
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
    tableauQtouch[i].update(); 
    if (tableauQtouch[i].getState() == 1){
      Tab_state[i] = 1;
      //Serial.print("Ignore Note QT ");Serial.print(i); Serial.print(" : ");Serial.println(tableauQtouch[i].IgnoreNote);
    }
    else
      Tab_state[i] = 0;
    //Serial.print("Ignore Note QT ");Serial.print(i); Serial.print(" : ");Serial.println(tableauQtouch[i].IgnoreNote);
  }
}

// Piezo's timer callback function
void TimerCallback0(){
  int Nqt = 0;  // number of non-actif qtouch zones
  if (recheckQt == 1){
    qTouchLoop();
    sendNote = 1;
  }
  for (int i = 0; i < 7; i ++){
    if (Tab_state[i] == 1 && tableauQtouch[i].IgnoreNote == 0){
      Piezo.update((uint8_t)(60+i));
        if(DoneSendNote[i] == 1){
          //midiEventPacket_t event = {0x09, 0x90 | 0, 60+i, 65};
          //MidiUSB.sendMIDI(event);
          tableauQtouch[i].IgnoreNote = 1;
          DoneSendNote[i] = 0;
        }
    }
    else
      Nqt = Nqt+1;
  }
  if (Nqt == 7){
      Piezo.update((uint8_t)(0));
  }
}

// Piezo's timer setup
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
