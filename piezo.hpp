#include "MIDIUSB.h"
#define UNDERTHRESHOLD 0
#define SIGNAL 1
#define RISING 2
#define PEAK 3
#define FALLING 4
#define pin_t int

#include <iostream>
using namespace std;

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
		threshold = 50;
    state = UNDERTHRESHOLD;
    prevstate = UNDERTHRESHOLD;
		debounceTime = 80;
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
		void update();
		void playnote(int);
		void piezoNote();
	private:
		MIDIAddress _address;
    pin_t _pin;
    unsigned long piezoTimer;
    int prevpiezoRead;
};
