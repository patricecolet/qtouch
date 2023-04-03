#include "distance.h"

distancePB::distancePB(byte channel) {
  distance = Adafruit_VL53L0X();
  _channel = channel;
};

void distancePB::begin() {
  distance.begin();
};

void distancePB::update() {
  distance.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  pitchbend = measure.RangeMilliMeter;
  if (measure.RangeStatus != 4) sendPitchbend();
};
  
void distancePB::sendPitchbend() {
  byte lowValue = pitchbend & 0x7F;
  byte highValue = pitchbend >> 7;
  midiEventPacket_t event ={0x0E, 0xE0 | _channel, lowValue, highValue};
  MidiUSB.sendMIDI(event);
};
