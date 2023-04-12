#include "distance.h"
#define HIGHEST_RANGE_MM 800
#define LOWEST_RANGE_MM 10
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
    if ( measure.RangeMilliMeter >= LOWEST_RANGE_MM && measure.RangeMilliMeter <= HIGHEST_RANGE_MM) sendPitchbend();
  }

};
  
void distancePB::sendPitchbend() {
//    Serial.println(pitchbend);
  byte lowValue = pitchbend & 0x7F;
  byte highValue = pitchbend >> 7;
  midiEventPacket_t event ={0x0E, 0xE0 | _channel, lowValue, highValue};
  MidiUSB.sendMIDI(event);
};
