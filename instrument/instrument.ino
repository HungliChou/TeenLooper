// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/

///////////////////////////////////
// copy the Design Tool code here
///////////////////////////////////
#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlayMemory          playMem1;       //xy=105,73
AudioPlayMemory          playMem2;       //xy=107,118
AudioPlayMemory          playMem3;       //xy=107,118
AudioPlaySdWav           playSdWav1;     //xy=110,212
//AudioPlaySdWav           playSdWav2;     //xy=112,250
//AudioPlaySdWav           playSdWav3;     //xy=116,286
AudioPlaySdRaw           playSdRaw1;     //xy=123,372
AudioPlaySdRaw           playSdRaw2;     //xy=157,438
AudioSynthSimpleDrum     drum3;          //xy=424,310
AudioSynthSimpleDrum     drum1;          //xy=424,310
AudioSynthSimpleDrum     drum2;          //xy=424,310
AudioMixer4              mixer2;         //xy=311,205
AudioMixer4              mixer1;         //xy=312,98
AudioMixer4              mixer3;         //xy=482,209
AudioRecordQueue         queue1;         //xy=616,100
AudioOutputI2S           i2s1;           //xy=647,251

AudioConnection          patchCord1(playMem1, 0, mixer1, 0);
AudioConnection          patchCord2(playMem2, 0, mixer1, 1);
AudioConnection          patchCord3(playSdWav1, 0, mixer1, 2);
AudioConnection          patchCord4(drum1, 0, mixer2, 0);
AudioConnection          patchCord5(drum3, 0, mixer2, 1);
AudioConnection          patchCord6(drum2, 0, mixer2, 2);
AudioConnection          patchCord7(playSdRaw1, 0, mixer3, 2);
AudioConnection          patchCord8(playSdRaw2, 0, mixer3, 3);
AudioConnection          patchCord9(mixer2, 0, mixer3, 1);
AudioConnection          patchCord10(mixer1, 0, mixer3, 0);
AudioConnection          patchCord11(mixer3, 0, i2s1, 0);
AudioConnection          patchCord12(mixer3, 0, i2s1, 1);
AudioConnection          patchCord14(playMem3, 0, i2s1, 0);
AudioConnection          patchCord15(playMem3, 0, i2s1, 1);
AudioConnection          patchCord13(mixer1, queue1);
AudioControlSGTL5000     sgtl5000_1;     //xy=253,519
// GUItool: end automatically generated code

//const int myInput = AUDIO_INPUT_MIC;  //uncomment this to use it for mic input

File f1;                        // record file
int mode = 0;                   // 0=stopped, 1=recording, 2=playing
bool soundMode = false;         // false->1st row of sound, true->2nd row of sound

unsigned long play1Interval;    //loop interval
unsigned long play1Timer;       //loop timer for playing record repeatedly

//Set pin: begin
int ledSwitch_Pin = 15;         
int ledRecord_Pin = 20;
int ledPlay1_Pin = 21;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// Bounce objects to read pushbuttons 
Bounce button0 = Bounce(0, 15);
Bounce button1 = Bounce(1, 15);  // 15 ms debounce time
Bounce button2 = Bounce(2, 15);
Bounce buttonSoundSwitch = Bounce(17, 15);
Bounce buttonPlay1 = Bounce(5, 15);
Bounce buttonRecord = Bounce(8, 15);
//Set pin: end

void setup() {
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(ledSwitch_Pin, OUTPUT);
  pinMode(ledRecord_Pin, OUTPUT);
  pinMode(ledPlay1_Pin, OUTPUT); 
  
  AudioMemory(10);
  sgtl5000_1.enable();
  //sgtl5000_1.inputSelect(myInput);  //uncomment this to use it for mic input
  sgtl5000_1.volume(1);

  //set volume for each input for mixers
  mixer1.gain(0, 0.4);
  mixer1.gain(1, 0.4);
  mixer1.gain(2, 0.4);
  mixer2.gain(0, 0.6);
  mixer2.gain(1, 0.6);
  mixer2.gain(2, 0.6);
  mixer3.gain(0, 0.4);
  mixer3.gain(1, 0.4);
  mixer3.gain(2, 0.4);
  mixer3.gain(3, 0.4);
  mixer3.gain(4, 0.4);

  //Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  //set drum properties
  drum3.frequency(500);
  drum3.length(1000);
  drum3.secondMix(1.0);
  drum3.pitchMod(0.5);
  drum2.frequency(550);
  drum2.length(1000);
  drum2.secondMix(1.0);
  drum2.pitchMod(0.5);
  drum1.frequency(600);
  drum1.length(1000);
  drum1.secondMix(1.0);
  drum1.pitchMod(0.5);
}

void loop() {
  // Update all the button objects
  button0.update();
  button1.update();
  button2.update();
  buttonSoundSwitch.update();
  buttonRecord.update();
  buttonPlay1.update();

  // Check when pressing "sound" buttons
  if (button0.fallingEdge()) {
    if (!soundMode) {
      playMem1.play(AudioSampleTomtom);
    } else {
     drum3.noteOn();
    }
  }

  if (button1.fallingEdge()) {
    if (!soundMode) {
      playSdWav1.play("HiHatt.WAV");
    } else {
      drum2.noteOn();
    }
  }
  if (button2.fallingEdge()) {
    if (!soundMode) {
      playMem1.play(AudioSampleSnare);
    } else {
      drum1.noteOn();
    }
  }

  //Check when pressing "sound switch" button
  if(buttonSoundSwitch.fallingEdge()){
    soundMode = !soundMode;
    if(soundMode)
    {
      digitalWrite(ledSwitch_Pin, HIGH);   // turn the LED on (HIGH is the voltage level)
      }
      else
      {
        digitalWrite(ledSwitch_Pin, LOW);   // turn the LED on (HIGH is the voltage level)
        }
  }

  //Check when pressing "play" button
  if(buttonPlay1.fallingEdge())
  {
    if(SD.exists("RECORD.RAW")){
      if (mode == 2) stopPlaying();
      else if (mode == 0) startPlaying();
    }
  }

  //Check when pressing "record" button
  if (buttonRecord.fallingEdge()) {
   Serial.print("record");
   if (mode == 1) stopRecording();
   else if (mode == 2) stopPlaying();
   else if (mode == 0) startRecording();
  }

  // If we're playing or recording, carry on...
  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
    
  }
}

void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD.RAW")) {
    // The old file must be deleted before new data is written.
    SD.remove("RECORD.RAW");
  }
  f1 = SD.open("RECORD.RAW", FILE_WRITE);
  if (f1) {
    queue1.begin();
    digitalWrite(ledRecord_Pin,HIGH);
    mode = 1;
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    elapsedMicros usec = 0;
    f1.write(buffer, 512);
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      f1.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    f1.close();
  }
  digitalWrite(ledRecord_Pin,LOW);
  mode = 0;
}

void startPlaying() {
  if(SD.exists("RECORD.RAW")){
    Serial.println("startPlaying");
    playSdRaw1.play("RECORD.RAW");
    digitalWrite(ledPlay1_Pin,HIGH);
    play1Interval = playSdRaw1.lengthMillis();
    play1Timer = millis();
    mode = 2;
  }
}

void continuePlaying() {
  if((millis()-play1Timer)>=play1Interval){
    Serial.println(play1Interval);
    playSdRaw1.play("RECORD.RAW");
    play1Timer = millis();
    }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playSdRaw1.stop();
  digitalWrite(ledPlay1_Pin,LOW);
  mode = 0;
}

