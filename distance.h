#include "Adafruit_VL53L0X.h"
#include "MIDIUSB.h"


class distancePB {
  public:
    distancePB(byte);
    bool begin();
    void update();
    void sendController();
//  private:
    Adafruit_VL53L0X distance;
    byte _channel;
    bool Detect;
    bool prevDetect;
    uint16_t distanceRead;
    uint16_t ControllerValue;
    VL53L0X_RangingMeasurementData_t measure;
    bool inRange;
};
