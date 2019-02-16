#include "pitches.h"
#include <SoftwareSerial.h>

// define the pins used
#define VS1053_RX  2 // This is the pin that connects to the RX pin on VS1053

#define VS1053_RESET 9 // This is the pin that connects to the RESET pin on VS1053
// If you have the Music Maker shield, you don't need to connect the RESET pin!

// If you're using the VS1053 breakout:
// Don't forget to connect the GPIO #0 to GROUND and GPIO #1 pin to 3.3V
// If you're using the Music Maker shield:
// Don't forget to connect the GPIO #1 pin to 3.3V and the RX pin to digital #2

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
#define VS1053_GM1_OCARINA 80
#define VS1053_GM1_CLARINET 72
#define VS1053_GM1_TRUMPET 57
#define VS1053_GM1_ACOUSTIC_GRAND_PIANO 1

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

SoftwareSerial VS1053_MIDI(0, 2); // TX only, do not use the 'rx' side
// on a Mega/Leonardo you may have to change the pin to one that 
// software serial support uses OR use a hardware serial port!


// constants won't change. They're used here to
// set pin numbers:
const int buttonOnePin = 7;     // the number of the pushbutton pin
const int ledOnePin =  9;      // the number of the LED pin

const int buttonTwoPin = 6;
const int ledTwoPin = 11;

const int buttonThreePin = 4;
const int ledThreePin = 12;

const int buttonFourPin = 5;
const int ledFourPin = 13;
 
// variables will change:
int buttonOneState = 0;         // variable for reading the pushbutton status
int buttonTwoState = 0;
int buttonThreeState = 0;
int buttonFourState = 0;


boolean notes[10] = {false, false, false, false, false, false, false, false, false, false}; // this checks to see if a note is currently playing

// change this array to change note values
byte noteValue[10] = {60, 64, 67, 72, 0, 0, 0, 0, 0, 0}; // put the values of the 10 notes here DO NOT MODIFY THIS WITH FUNCTIONS

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledOnePin, OUTPUT);
  pinMode(ledTwoPin, OUTPUT);
  pinMode(ledThreePin, OUTPUT);
  pinMode(ledFourPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonOnePin, INPUT);
  pinMode(buttonTwoPin, INPUT);
  pinMode(buttonThreePin, INPUT);
  pinMode(buttonFourPin, INPUT);
 
  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'
  
  pinMode(VS1053_RESET, OUTPUT);
  digitalWrite(VS1053_RESET, LOW);
  delay(10);
  digitalWrite(VS1053_RESET, HIGH);
  delay(10);
  
  midiSetChannelBank(0, VS1053_BANK_MELODY);
  midiSetInstrument(0, VS1053_GM1_ACOUSTIC_GRAND_PIANO);
  midiSetChannelVolume(0, 120);

}

void loop() {
  // read the state of the pushbutton value:
  buttonOneState = digitalRead(buttonOnePin); //high or low
  buttonTwoState = digitalRead(buttonTwoPin);
  buttonThreeState = digitalRead(buttonThreePin);
  buttonFourState = digitalRead(buttonFourPin);
 
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:

  // Note 1
  if (buttonOneState == HIGH) {
    // turn LED on:
    digitalWrite(ledOnePin, HIGH);
    playMyNote(noteValue[0], 0);
  } else {
    // turn LED off:
    digitalWrite(ledOnePin, LOW);
    stopMyNote(noteValue[0], 0);
  }

  // Note 2
  if (buttonTwoState == HIGH) {
    // turn LED on:
    digitalWrite(ledTwoPin, HIGH);
    playMyNote(noteValue[1], 1);
  } else {
    // turn LED off:
    digitalWrite(ledTwoPin, LOW);
    stopMyNote(noteValue[1], 1);  
  }

  // Note 3
  if (buttonThreeState == HIGH) {
    // turn LED on:
    digitalWrite(ledThreePin, HIGH);
    playMyNote(noteValue[2], 2);
  } else {
    // turn LED off:
    digitalWrite(ledThreePin, LOW);
    stopMyNote(noteValue[2], 2);
  }
  
  // Note 4
  if (buttonFourState == HIGH) {
    // turn LED on:
    digitalWrite(ledFourPin, HIGH);
    playMyNote(noteValue[3], 3); //should input a numeric note value, and the index of note status
  } else {
    // turn LED off:
    stopMyNote(noteValue[3], 3);
    digitalWrite(ledFourPin, LOW);
  } 
}
void playMyNote(byte note, int boolValue){
    // check to see if note is playing
    if(!notes[boolValue]){
      //play note
      noteOn(0, note, 120);
      // array reflects note playing
      notes[boolValue] = true;
    }
}
void stopMyNote(byte note, int boolValue){
    if(notes[boolValue]){
      noteOff(0, note, 120);
      notes[boolValue] = false;
    }
}
void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 120) return;
  
  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);  
  VS1053_MIDI.write(inst);
}


void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 120) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 120) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

void noteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 120) return;
  if (vel > 120) return;
  
  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void noteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 120) return;
  if (vel > 120) return;
  
  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}
