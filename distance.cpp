#include <stdint.h>
#include "distance.h"
#define HIGHEST_RANGE_MM 400
#define LOWEST_RANGE_MM 100
#define CONTROLER_LSB 46
#define CONTROLER_MSB 14

distancePB::distancePB(byte channel) : filter(filterAmount) {
  Adafruit_VL53L0X distance = Adafruit_VL53L0X();
  _channel = channel;
};

bool distancePB::begin() {
  
  Serial.println("VL53L0X begin");
  if(!distance.begin()) {
  Serial.println("begin failed");
    return 0;
  }
  else {
      Serial.println("begin succeed");
  
  distance.startRangeContinuous();
  ControllerValue = 16383;
  sendController();
  return 1;
  }
};

void distancePB::update() {
//  distance.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  distanceRead = distance.readRange();

//      Serial.println(distanceRead);
  if (measure.RangeStatus != 4) {
    if ( distanceRead >= LOWEST_RANGE_MM && distanceRead <= HIGHEST_RANGE_MM) {
      Detect = 1;
    }
    else{
      Detect = 0;
//      Serial.println("NOT IN RANGE");
    }
  }
  else
      Detect = 0;
  if (Detect == 1){
    double filteredDistance = filter.addSample((double)(distanceRead));
    ControllerValue = 16383 * (filteredDistance - LOWEST_RANGE_MM) / (HIGHEST_RANGE_MM - LOWEST_RANGE_MM);
    if (ControllerValue < 0)
      ControllerValue = 0;
  sendController();
  }
  // else if (prevDetect == 1 && Detect == 0) {
  //   ControllerValue = 16383;
  //     sendController();
  // }

  prevDetect = Detect;
};

void distancePB::sendController() {
    byte lowValue = ControllerValue & 0x7F;
    byte highValue = ControllerValue >> 7;
    midiEventPacket_t event_lsb = {0x0B, 0xB0 |  _channel, CONTROLER_LSB, lowValue};
    MidiUSB.sendMIDI(event_lsb);
    midiEventPacket_t event_msb = {0x0B, 0xB0 |  _channel, CONTROLER_MSB, highValue};
    MidiUSB.sendMIDI(event_msb);
}