//Use interrupt to optimize analogRead
//#include "Adafruit_ZeroTimer.h"
//#define COMPARE 48000

#include "MIDIUSB.h"
#define UNDERTHRESHOLD 0
#define SIGNAL 1
#define RISING 2
#define PEAK 3
#define FALLING 4
#define pin_t int

#include <iostream>
using namespace std;

//extern uint8_t velopiezo;

template<typename T> 
struct piezoState 
{ 
	T threshold;
	T state;
	T prevstate;
	T debounceTime;
	T sensitivity;
	T peak;
	piezoState()
	{
		threshold = 35;
    state = UNDERTHRESHOLD;
    prevstate = UNDERTHRESHOLD;
		debounceTime = 25;
		sensitivity = 1023;
		peak = 0;
	}
};

class piezo {
	public:
		struct MIDIAddress {
			uint8_t address : 7;
			uint8_t channel : 4;
		};
		piezo(pin_t, MIDIAddress);
		void update(uint8_t);
		void piezoNote(uint8_t);
    void playnote(int);
	private:
    //Adafruit_ZeroTimer zerotimer;
		MIDIAddress _address;
    uint8_t velocity;
    pin_t _pin;
    unsigned long piezoTimer;
    int prevpiezoRead;
    //int SendVelo;
    piezoState<int> Piezo;
};
