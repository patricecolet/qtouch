#include "Adafruit_VL53L0X.h"
#include "MIDIUSB.h"


class distancePB {
  public:
    distancePB(byte);
    void begin();
    void update();
    void sendPitchbend();
  private:
    Adafruit_VL53L0X distance;
    byte _channel;
    uint16_t pitchbend;
    VL53L0X_RangingMeasurementData_t measure;
    bool inRange;
};
