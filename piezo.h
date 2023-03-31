#include "MIDIUSB.h"
#define PIEZO A3
#define RISING 1
#define FALLING 0

struct piezoState{
  int threshold;
  bool state;
  int attackTime;
  int sensitivity;
  int volume;
  uint8_t note;
  uint8_t channel;
} piezo;

void initPiezo() {
  piezo.threshold = 50; // noise threshold amount
  piezo.state = FALLING;
  piezo.debounceTime = 50; // time between rising and falling state
  piezo.sensitivity = 100;
  piezo.volume = 0;
  piezo.note = 48; //MIDI note on
  piezo.channel = 0;
}

void playnote() {
  float velocity = (piezo.velocity / float(piezo.sensitivity - piezo.threshold)) * 127;
  if (velocity > 127) velocity = 127;
  if (velocity > )
}

void piezoNoteOn() {
      uint8_t velocity = 127 * piezo.volume / 1023;
      midiEventPacket_t event = {0x09, 0x90 | piezo.channel, piezo.note, velocity};
      MidiUSB.sendMIDI(event);
};
void piezoNoteOff() {
      midiEventPacket_t event = {0x08, 0x80 | piezo.channel, piezo.note, 0};
      MidiUSB.sendMIDI(event);
};

