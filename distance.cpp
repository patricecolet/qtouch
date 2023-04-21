#include <stdint.h>
#include "distance.h"
#define HIGHEST_RANGE_MM 800
#define LOWEST_RANGE_MM 10
#define CONTROLER_LSB 98
#define CONTROLER_MSB 99

distancePB::distancePB(byte channel) {
  Adafruit_VL53L0X distance = Adafruit_VL53L0X();
  _channel = channel;
};

bool distancePB::begin() {
  
  Serial.println("Adafruit begin");
  if(!distance.begin()) {
    
  Serial.println("begin failed");
    return 0;
  }
  else {
      Serial.println("begin succeed");
    return 1;
  }
};

void distancePB::update() {
  distance.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  pitchbend = measure.RangeMilliMeter;
  if (measure.RangeStatus != 4) {
    if ( measure.RangeMilliMeter >= LOWEST_RANGE_MM && measure.RangeMilliMeter <= HIGHEST_RANGE_MM)
      Detect = 1;
    else{
      Detect = 0;
      //Serial.println("NOT IN RANGE");
    }
  }
  else
      Detect = 0;
  Serial.print("Detect : ");Serial.println(Detect);
  Serial.print("prevDetect : ");Serial.println(prevDetect);
  sendController();
  prevDetect = Detect;
};
  
void distancePB::sendController() {
  if (Detect == 1){
    ControllerValue = HIGHEST_RANGE_MM - pitchbend;
    if (ControllerValue < 0)
      ControllerValue = 0;
    byte lowValue = ControllerValue & 0x7F;
    byte highValue = ControllerValue >> 7;
    //midiEventPacket_t event ={0x0E, 0xE0 | _channel, lowValue, highValue};
    midiEventPacket_t event_lsb = {0x0B, 0xB0 |  _channel, CONTROLER_LSB, lowValue};
    MidiUSB.sendMIDI(event_lsb);
    midiEventPacket_t event_msb = {0x0B, 0xB0 |  _channel, CONTROLER_MSB, highValue};
    MidiUSB.sendMIDI(event_msb);
  }
  else if (prevDetect == 1 && Detect == 0) {
    //Serial.println("NOT IN RANGE");
    midiEventPacket_t event_lsb = {0x0B, 0xB0 |  _channel, CONTROLER_LSB, 0};
    MidiUSB.sendMIDI(event_lsb);
    midiEventPacket_t event_msb = {0x0B, 0xB0 |  _channel, CONTROLER_MSB, 0};
    MidiUSB.sendMIDI(event_msb);
  }
};
