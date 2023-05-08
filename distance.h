#include "Adafruit_VL53L0X.h"
#include "MIDIUSB.h"
#include "MovingAverage.h"

class distancePB{
  public:
    distancePB(byte);
    bool begin();
    void update();
    void sendController();
  private:
    Adafruit_VL53L0X distance;
    const int filterAmount = 5;
    MovingAverage filter;
    byte _channel;
    bool Detect;
    bool prevDetect;
    uint16_t distanceRead;
    uint16_t ControllerValue;
    VL53L0X_RangingMeasurementData_t measure;
    bool inRange;
};
