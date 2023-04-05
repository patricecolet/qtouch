#include "MIDIUSB.h"
#define PIEZO A3
#define UNDERTHRESHOLD 0
#define SIGNAL 1
#define RISING 2
#define PEAK 3
#define FALLING 4

struct piezoState{
  int threshold;
  int state;
  int prevstate;
  int debounceTime;
  int sensitivity;
  int peak;
  uint8_t note;
  uint8_t channel;
} piezo;

void initPiezo() {
  piezo.threshold = 50;     // noise threshold amount
  piezo.state = UNDERTHRESHOLD;
  piezo.prevstate = UNDERTHRESHOLD;
  piezo.debounceTime = 100;  // time between rising and falling state
  piezo.sensitivity = 1023;  // Maximum input range
  piezo.peak = 0;
  piezo.note = 48;          //MIDI note on
  piezo.channel = 0;
}

void playnote(int piezoRead) {
  float velocity = (piezoRead / float(piezo.sensitivity - piezo.threshold)) * 127;
  if (velocity > 127) velocity = 127;
  if (velocity > piezo.peak) piezo.peak = velocity;
}

void piezoNoteOn() {
      midiEventPacket_t event = {0x09, 0x90 | piezo.channel, piezo.note, piezo.peak};
      MidiUSB.sendMIDI(event);
};
void piezoNoteOff() {
      midiEventPacket_t event = {0x08, 0x80 | piezo.channel, piezo.note, 0};
      MidiUSB.sendMIDI(event);
};
