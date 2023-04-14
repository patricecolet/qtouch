// This code works only on SAMD21

//Use interrupt to optimize analogRead
#include "Adafruit_ZeroTimer.h"
int COMPARE = 48000/2;

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
  {48, 0 }     // Note Number 48 on MIDI channel 1
//  3
 };
//global variable for piezo
int qtouchActif = 0;
int veloPiezo = 0;  // variable for the piezo's velocity 

// timer to prioritize piezo
Adafruit_ZeroTimer zerotimer = Adafruit_ZeroTimer(3);
void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}

// distance sensor uses Adafruit_VL53L0X library
distancePB Distance(0); // MIDI channel 1 is '0'
long distanceTimer;

// flash storage to stock the piezo's velocity
FlashStorage(my_flash_store, int);

void setup() {
  distanceTimer = millis();
  Serial.begin(115200);
  qTouchBegin();
  timerBegin();
  Distance.begin();
  my_flash_store.write(qtouchActif);

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
  if (millis() - distanceTimer > 100) {
    distanceTimer = millis();
    Distance.update();
  }
//  qTouchLoop();
//  if (tableauQtouch[0].getState() == 1) 
//    Serial.println(tableauQtouch[0].getState());

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
  if (tableauQtouch[i].getState() == 1) 
    my_flash_store.write(i+1);
  }
}

// the timer callback
void TimerCallback0(void){
  //piezoread = analogRead(A3);
  //Serial.println(piezoread);
  qTouchLoop();
  veloPiezo = Piezo.update();
  qtouchActif = my_flash_store.read()-1;
  if (veloPiezo != 0 && qtouchActif != 0){
    Serial.print("QTOUCH : ");Serial.println(qtouchActif);
    tableauQtouch[qtouchActif].sendNoteOn(veloPiezo);
  }
  //Serial.println(veloPiezo);
  // for (int i = 0; i < 7; i ++){
  //  tableauQtouch[i].setVelocity(veloPiezo);   
  // }
}

void timerBegin(){ 
  tc_clock_prescaler prescaler = TC_CLOCK_PRESCALER_DIV1;

  zerotimer.enable(false);
  zerotimer.configure(prescaler,       // prescaler
          TC_COUNTER_SIZE_16BIT,       // bit width of timer/counter
          TC_WAVE_GENERATION_MATCH_PWM // frequency or PWM mode
          );

  zerotimer.setCompare(0, COMPARE);
  zerotimer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, TimerCallback0);
  zerotimer.enable(true);
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
