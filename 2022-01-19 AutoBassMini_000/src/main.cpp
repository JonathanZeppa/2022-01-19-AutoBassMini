#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>
#include <MIDI.h>
#include <Bounce2.h>
#include <RotaryEncoder.h>


#define r01STEPS 1
#define r01MIN 0
#define r01MAX 35
#define r02STEPS 1
#define r02MIN 0
#define r02MAX 24

String Key1[] = {
  "C 3", "C#3", "D 3", "D#3", "E 3", "F 3", "F#3", "G 3", "G#3", "A 3", "A#3", "B 3",
  "C 4", "C#4", "D 4", "D#4", "E 4", "F 4", "F#4", "G 4", "G#4", "A 4", "A#4", "B 4",
  "C 5", "C#5", "D 5", "D#5", "E 5", "F 5", "F#5", "G 5", "G#5", "A 5", "A#5", "B 5"
  };
// Add 3 octave here, {"C 4", "C#4"}  36, 48, 60
int keyNUM[] = {
  -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
  };
int transpose = 0;

// Fix mode overlap with KEy



String Mode2[] = {
"Major       ", "Dorian      ", "Phrygian    ", "Lydian      ", "Mixolydian  ", "Minor       ", "Locrian     ", 

"HarmonicMIN ", "Locrian #6  ", "Ionian #5   ", "Dorian #4   ", "Phrygian DM ", "Lydian #2   ", "UltraLocrian",

"D.Harm.MAJ  ", "Lydian #2 #6", "UltraPhryg  ", "HungarianMIN", "Asian       ", "Ionian #2#5 ", "Loc bb3 bb7 ", 

"HarmonicMAJ ", "HungarianMAJ", "Lydian #9   ", "Chroma 5    " 
  };

int modeVar = 0;// filled in from the encoder

char modeSelect2[25][12][2] = { // {note, chord type}
  
  {{0,0}, {0,0}, {2, 1}, {2, 1}, {4,1}, {5,0}, {5,0}, {7,0}, {7,0}, {9,1}, {9,1}, {11,2}}, // 0. Ionian
  {{0,1}, {1,0}, {2, 1}, {2, 1}, {3,0}, {5,0}, {5,0}, {7,1}, {8,0}, {9,2}, {10,5}, {10,0}}, // 1. Dorian
  {{0,1}, {0,1}, {1, 0}, {1, 0}, {3,0}, {5,1}, {5,1}, {7,2}, {7,2}, {8,0}, {8,0}, {10,1}}, // 2. Phrygian
  {{0,0}, {0,0}, {2, 0}, {2, 0}, {4,1}, {6,2}, {6,2}, {7,0}, {7,0}, {9,1}, {9,1}, {11,1}}, // 3. Lydian
//    0      1      2      3      4      5      6      7     8       9      10      11     12
// {"MAJ", "MIN", "DIM", "AUG", "5th", "7#9", "b5", "sus4", "S4b5", "Loc", "Phr", "Lyd", "DMb3"};

  
  {{0,0}, {0,0}, {2, 1}, {2, 1}, {4,2}, {5,0}, {7,0}, {7,1}, {8,0}, {9,1}, {10,1}, {10,0}}, // 4. Mixolydian

//           #               #                    #             #             #
//   1              2              3      4             5              6             7
//   0      1       2      3       4      5      6      7      8       9      10     11 
  {{0,1}, {0,0}, {2, 2}, {2, 0}, {3,0}, {5,1}, {5,0}, {7,1}, {7,0}, {8,0}, {8,0}, {10,0}}, // 5. Aeolian
  {{0,2}, {0,2}, {1, 0}, {1, 0}, {3,1}, {5,1}, {5,1}, {6,0}, {6,0}, {8,0}, {8,0}, {10,1}},  // 6. Locrian 
  {{0,1}, {0,1}, {2, 2}, {2, 2}, {3,3}, {5,1}, {5,1}, {7,0}, {7,0}, {8,0}, {8,0}, {11,2}}, // 7. Harmonic Minor!, Aeolian Natural 7
  {{0,2}, {0,2}, {1, 3}, {1, 0}, {3,1}, {5,0}, {5,1}, {6,0}, {6,0}, {9,2}, {8,0}, {10,1}}, // 8. Locrian #6.
  {{0,3}, {0,0}, {2, 1}, {2, 0}, {4,0}, {5,0}, {5,0}, {7,2}, {7,0}, {9,1}, {9,1}, {11,2}}, // 9. Ionian #5.
  {{0,1}, {1,0}, {2, 0}, {2, 1}, {3,0}, {6,2}, {6,0}, {7,1}, {8,0}, {9,2}, {10,5}, {10,3}},// 10. Dorian #4.
  {{0,0}, {1,1}, {1, 2}, {1, 0}, {4,2}, {5,1}, {5,2}, {7,2}, {7,3}, {8,3}, {8,2}, {10,1}}, // 11. Phrygian Dominant.
  {{0,0}, {0,0}, {3, 2}, {3, 0}, {4,1}, {6,2}, {6,2}, {7,3}, {7,0}, {9,1}, {9,1}, {11,0}}, // 12. Lydian #2.
  {{0,2}, {0,2}, {1, 1}, {1, 0}, {3,2}, {5,3}, {5,1}, {6,1}, {6,0}, {8,0}, {8,0}, {9,0}},  // 13. UltraLocrian, Superlocrian bb7.
  {{0,0}, {1,7}, {1, 0}, {4, 11}, {4,1}, {5,1}, {5,2}, {7,6}, {7,9}, {8,3}, {11,9}, {11,12}}, // 14. Double harmonic major 1 ♭2  3 4 5 ♭6  7 8
  {{0,0}, {1,0}, {3, 1}, {3, 1}, {4,1}, {6,6}, {6,0}, {7,3}, {8,0}, {10,12}, {10,5}, {11,0}}, // 15. Lydian♯2♯6
  {{0,1}, {0,1}, {1, 1}, {1, 0}, {3,6}, {4,3}, {5,1}, {7,12}, {7,2}, {8,0}, {8,0}, {9,0}},    // 16. Ultraphrygian  1 ♭2  ♭3  ♭4  5 ♭6  double flat7  8
  {{0,1}, {0,0}, {2, 6}, {2, 0}, {3,3}, {6,12}, {6,2}, {7,0}, {7,0}, {8,0}, {9,1}, {11,1}},   // 17. Hungarian minor, Double Harmonic Minor
  {{0,6}, {0,0}, {1, 3}, {1, 1}, {4,12}, {5,0}, {5,0}, {6,0}, {7,1}, {9,1}, {9,1}, {10,1}},   // 18. Asian  1 ♭2  3 4 ♭5  6 ♭7  8
  {{0,3}, {0,1}, {3, 12}, {3, 2}, {4,0}, {5,0}, {5,1}, {7,1}, {7,1}, {9,1}, {11,0}, {11,6}},  // 19. Ionian ♯2 ♯5  1 ♯2  3 4 ♯5  6 7 8
  {{0,12}, {0,1}, {1, 0}, {2, 2}, {2,0}, {5,1}, {5,1}, {6,1}, {6,0}, {8,6}, {8,0}, {9,3}},    // 20. Locrian bb3 bb7  1 ♭2  double flat3  4 ♭5  ♭6
  {{0,0}, {0,1}, {2, 2}, {2, 0}, {4,1}, {5,1}, {5,1}, {7,0}, {8,0}, {8,3}, {10,0}, {11,2}},  //?? 21. Harmonic Major 1 2 3 4 5 ♭6 7
  {{0,0}, {0,0}, {3, 2}, {3, 2}, {4,2}, {6,2}, {6,4}, {7,3}, {8,4}, {9,1}, {10,4}, {10,3}}, // 22. Blues Exotic - Hungarian Major
  {{0,1}, {0,0}, {3, 2}, {3, 3}, {4,1}, {6,3}, {6,8}, {7,3}, {9,11}, {9,1}, {11,7}, {11,0}},// 23. Lydian #9, 
  {{0,4}, {1,4}, {2, 4}, {3, 4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4}, {9,4}, {10,4}, {11,4}} // 24. Chromatic with 5th/Oct

};

int s1 = 0; 
int s2 = 0;
int chord = 0;

// Button array for octave control for bass and treble? [FUTURE]
int octaves[] = {12,24,36,48,60,72,84,96};
int oct = octaves[4];
int dOct = 24; // octaves[3];
int root = 0;
int padNote = 36;
int rootPlay = 0;
int bass = 24;
int bass2 = 28;
int bass3 = 31;
//int troot = 0;
//int dOctaves[] = {12,24,36,48,60,72,84,96};

String chordName[13] = {"MAJ ", "MIN ", "DIM ", "AUG ", "5th ", "7#9 ", "b5  ", "sus4", "S4b5", "Loc ", "Phr ", "Lyd ", "DMb3"};

//int addOct = 0;

int seqNote = 36;

// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
RotaryEncoder r01(25, 26, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder r02(27, 14, RotaryEncoder::LatchMode::FOUR3);
// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
//RotaryEncoder encoder(25, 26, RotaryEncoder::LatchMode::TWO03);

// Last known rotary position.
int r01lastPos = -1;
int r02lastPos = -1;

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
//USBMIDI_CREATE_DEFAULT_INSTANCE(); not working

#define B01_PIN 32 //rename
#define B02_PIN 33 // Hold/Panic

int b01 = 1; // Chord
int b02 = 1; // Hold/Panic
int note = 36;

//Bounce b02Toggle= Bounce(TOGGLE02, BOUNCE_TIME);
Bounce2::Button b01Toggle = Bounce2::Button();
Bounce2::Button b02Toggle = Bounce2::Button();


void myNoteOn(byte channel, byte note, byte velocity) {
if  (channel == 10) { 
Serial.print ("From 10: ");
Serial.println (seqNote);
  seqNote = MIDI2.getData1(); 
if  (seqNote == 36) { 
      MIDI1.sendControlChange(123, 0, 2);
      //MIDI.sendControlChange(123, 0, 2);
      Serial.print ("seqNote:"); Serial.print (seqNote); 
      Serial.print (" Kick:"); ; Serial.println (bass); 
      MIDI1.sendNoteOn(bass, velocity, 2);
      //MIDI.sendNoteOn(bass, velocity, 2);
      
      }

  if  (seqNote == 41) { 
      MIDI1.sendControlChange(123, 0, 2);
      //MIDI.sendControlChange(123, 0, 2); 
      Serial.print ("s1:"); Serial.println (s1); Serial.print ("seqNote:"); Serial.print (seqNote); 
      Serial.print (" TOM-02:"); Serial.println (bass2); 
      MIDI1.sendNoteOn(bass2, velocity, 2);
      //MIDI.sendNoteOn(bass2, velocity, 2);
      
      }

  if  (seqNote == 45) { 
      MIDI1.sendControlChange(123, 0, 2);
      //MIDI.sendControlChange(123, 0, 2); 
      Serial.print ("s2:"); Serial.println (s2); Serial.print ("seqNote:"); Serial.print (seqNote); 
      Serial.print (" TOM-03:"); Serial.println (bass3); 
      MIDI1.sendNoteOn(bass3, velocity, 2);
      //MIDI.sendNoteOn(bass3, velocity, 2);
      
      } 


} // END Channel 10




if (channel == 1) {
if (b02 == 0) // Hold ON
{
    
    MIDI1.sendControlChange(123, 0, 1);
    //usbMIDI1.sendControlChange(120, 0, 1);
    //delay(5);
    if  (note == 36) { root = ((modeSelect2[modeVar][0][0])+oct); chord = modeSelect2[modeVar][0][1];}
    if  (note == 37) { root = ((modeSelect2[modeVar][1][0])+oct); chord = modeSelect2[modeVar][1][1];}
    if  (note == 38) { root = ((modeSelect2[modeVar][2][0])+oct); chord = modeSelect2[modeVar][2][1];} 
    if  (note == 39) { root = ((modeSelect2[modeVar][3][0])+oct); chord = modeSelect2[modeVar][3][1];}
    if  (note == 40) { root = ((modeSelect2[modeVar][4][0])+oct); chord = modeSelect2[modeVar][4][1];}
    if  (note == 41) { root = ((modeSelect2[modeVar][5][0])+oct); chord = modeSelect2[modeVar][5][1];}
    if  (note == 42) { root = ((modeSelect2[modeVar][6][0])+oct); chord = modeSelect2[modeVar][6][1];}
    if  (note == 43) { root = ((modeSelect2[modeVar][7][0])+oct); chord = modeSelect2[modeVar][7][1];}
    if  (note == 44) { root = ((modeSelect2[modeVar][8][0])+oct); chord = modeSelect2[modeVar][8][1];}
    if  (note == 45) { root = ((modeSelect2[modeVar][9][0])+oct); chord = modeSelect2[modeVar][9][1];}
    if  (note == 46) { root = ((modeSelect2[modeVar][10][0])+oct); chord = modeSelect2[modeVar][10][1];}
    if  (note == 47) { root = ((modeSelect2[modeVar][11][0])+oct); chord = modeSelect2[modeVar][11][1];}
    if  (note == 48) { root = ((modeSelect2[modeVar][0][0])+oct+12); chord = modeSelect2[modeVar][0][1];} //Octave, no need for change
    MIDI1.sendNoteOn(root+transpose, 100, channel); // root
    Serial.print ("HOLD ON:");
    Serial.println (root+transpose);
    rootPlay = root+transpose;
    if (b01 == 0) // CHORD ON
    { 
       switch (chord) {
          case 0: //maj
              s1 = 4;
              s2 = 7;
              break;
          case 1: //minor
              s1 = 3;
              s2 = 7;
              break;
          case 2: //dim
              s1 = 3;
              s2 = 6;
              break;   
          case 3: //aug
              s1 = 4;
              s2 = 8;
              break;     
          case 4: //5th-OCT
              s1 = 7;
              s2 = 12;
              break;
          case 5: //7#9
              s1 = 4;
              s2 = 10;
              break;
          case 6: //b5
              s1 = 4;
              s2 = 6;
              break;                  
          case 7: //sus4
              s1 = 5;
              s2 = 7;
              break;
          case 8: //S4b5
              s1 = 5;
              s2 = 7;
              break;             
          case 9: //LOC
              s1 = 1;
              s2 = 6;
              break;
          case 10: //PHR
              s1 = 1;
              s2 = 7;
              break;
          case 11: //LYD
              s1 = 6;
              s2 = 7;
              break;          
          case 12: //DMb3
              s1 = 2;
              s2 = 6;
              break;                                     
      } //END Switch chord

    Serial.println ("Chord ON:");

      MIDI1.sendNoteOn(root+s1+transpose, 100, channel); // dominant
      Serial.println (root+s1+transpose);
      MIDI1.sendNoteOn(root+s2+transpose, 100, channel); // tonic
      Serial.println (root+s2+transpose);
    } //END Chord ON
   } //END HOLD ON

else {
    MIDI1.sendControlChange(123, 0, 1); //maybe ditch this so polyphony can be used with scale
    //delay (5);
    if  (note == 36) { root = ((modeSelect2[modeVar][0][0])+oct); chord = modeSelect2[modeVar][0][1];}
    if  (note == 37) { root = ((modeSelect2[modeVar][1][0])+oct); chord = modeSelect2[modeVar][1][1];}
    if  (note == 38) { root = ((modeSelect2[modeVar][2][0])+oct); chord = modeSelect2[modeVar][2][1];} 
    if  (note == 39) { root = ((modeSelect2[modeVar][3][0])+oct); chord = modeSelect2[modeVar][3][1];}
    if  (note == 40) { root = ((modeSelect2[modeVar][4][0])+oct); chord = modeSelect2[modeVar][4][1];}
    if  (note == 41) { root = ((modeSelect2[modeVar][5][0])+oct); chord = modeSelect2[modeVar][5][1];}
    if  (note == 42) { root = ((modeSelect2[modeVar][6][0])+oct); chord = modeSelect2[modeVar][6][1];}
    if  (note == 43) { root = ((modeSelect2[modeVar][7][0])+oct); chord = modeSelect2[modeVar][7][1];}
    if  (note == 44) { root = ((modeSelect2[modeVar][8][0])+oct); chord = modeSelect2[modeVar][8][1];}
    if  (note == 45) { root = ((modeSelect2[modeVar][9][0])+oct); chord = modeSelect2[modeVar][9][1];}
    if  (note == 46) { root = ((modeSelect2[modeVar][10][0])+oct); chord = modeSelect2[modeVar][10][1];}
    if  (note == 47) { root = ((modeSelect2[modeVar][11][0])+oct); chord = modeSelect2[modeVar][11][1];}
    if  (note == 48) { root = ((modeSelect2[modeVar][0][0])+oct+12); chord = modeSelect2[modeVar][0][1]; //Octave, no need for change
    }
    MIDI1.sendNoteOn(root+transpose, 100, channel);
    Serial.print ("OFF Root:");
    Serial.println (root+transpose);
    rootPlay = root+transpose;

    // root
    if (b01 == 0) // CHORD ON
    { 
      switch (chord) {
          case 0: //maj
              s1 = 4;
              s2 = 7;
              break;
          case 1: //minor
              s1 = 3;
              s2 = 7;
              break;
          case 2: //dim
              s1 = 3;
              s2 = 6;
              break;   
          case 3: //aug
              s1 = 4;
              s2 = 8;
              break;     
          case 4: //5th-OCT
              s1 = 7;
              s2 = 12;
              break;
          case 5: //7#9
              s1 = 4;
              s2 = 10;
              break;
          case 6: //b5
              s1 = 4;
              s2 = 6;
              break;                  
          case 7: //sus4
              s1 = 5;
              s2 = 7;
              break;
          case 8: //S4b5
              s1 = 5;
              s2 = 7;
              break;             
          case 9: //LOC
              s1 = 1;
              s2 = 6;
              break;
          case 10: //PHR
              s1 = 1;
              s2 = 7;
              break;
          case 11: //LYD
              s1 = 6;
              s2 = 7;
              break;          
          case 12: //DMb3
              s1 = 2;
              s2 = 6;
              break;        
      } //end switch chord
      Serial.println ("Chord ON:");
      MIDI1.sendNoteOn(root+s1+transpose, 100, channel); // dominant
      Serial.println (root+s1+transpose);
      MIDI1.sendNoteOn(root+s2+transpose, 100, channel); // tonic
      Serial.println (root+s2+transpose);
    } //END Chord ON
   } //END else

} //channel 1
    bass = rootPlay-dOct;
    bass2 = rootPlay-dOct+s1;
    bass3 = rootPlay-dOct+s2;
}

void myNoteOff(byte channel, byte note, byte velocity) {

if (channel == 1) {
  // if (b03 == 0), sendNoteOff to channel 3 (or 15?). Will this work?  In theory, this way channel 1 never gets a note off message.
    if  (note == 36) { root = ((modeSelect2[modeVar][0][0])+oct); chord = modeSelect2[modeVar][0][1];}
    if  (note == 37) { root = ((modeSelect2[modeVar][1][0])+oct); chord = modeSelect2[modeVar][1][1];}
    if  (note == 38) { root = ((modeSelect2[modeVar][2][0])+oct); chord = modeSelect2[modeVar][2][1];} 
    if  (note == 39) { root = ((modeSelect2[modeVar][3][0])+oct); chord = modeSelect2[modeVar][3][1];}
    if  (note == 40) { root = ((modeSelect2[modeVar][4][0])+oct); chord = modeSelect2[modeVar][4][1];}
    if  (note == 41) { root = ((modeSelect2[modeVar][5][0])+oct); chord = modeSelect2[modeVar][5][1];}
    if  (note == 42) { root = ((modeSelect2[modeVar][6][0])+oct); chord = modeSelect2[modeVar][6][1];}
    if  (note == 43) { root = ((modeSelect2[modeVar][7][0])+oct); chord = modeSelect2[modeVar][7][1];}
    if  (note == 44) { root = ((modeSelect2[modeVar][8][0])+oct); chord = modeSelect2[modeVar][8][1];}
    if  (note == 45) { root = ((modeSelect2[modeVar][9][0])+oct); chord = modeSelect2[modeVar][9][1];}
    if  (note == 46) { root = ((modeSelect2[modeVar][10][0])+oct); chord = modeSelect2[modeVar][10][1];}
    if  (note == 47) { root = ((modeSelect2[modeVar][11][0])+oct); chord = modeSelect2[modeVar][11][1];}
    if  (note == 48) { root = ((modeSelect2[modeVar][0][0])+oct+12); chord = modeSelect2[modeVar][0][1];} //Octave, no need for change
    
    if (b02 == 0){ // HOLD ON
    MIDI1.sendNoteOff(root+transpose, velocity, 3); // Experiment to see if this line is necessary 
    if (b01 == 0){ // CHORD OFF
    MIDI1.sendNoteOff(root+s1+transpose, velocity, 3);
    MIDI1.sendNoteOff(root+s2+transpose, velocity, 3);}
    }

    if (b02 == 1){ // HOLD OFF
    MIDI1.sendNoteOff(root+transpose, velocity, channel); // Experiment to see if this line is necessary 
    if (b01 == 0){ // CHORD OFF
    MIDI1.sendNoteOff(root+s1+transpose, velocity, channel);
    MIDI1.sendNoteOff(root+s2+transpose, velocity, channel);}
    }
  
  } //END CHANNEL 1
if (channel == 10) {
  if  (seqNote == 36) { MIDI1.sendNoteOff(bass, velocity, 3);}
  if  (seqNote == 41) { MIDI1.sendNoteOff(bass2, velocity, 3);}
  if  (seqNote == 45) { MIDI1.sendNoteOff(bass3, velocity, 3);}
  // delay(250); gave the same problem as before, not allowing articulate playthrough.
  // I need to find something that interupts and resets the timer every time the note is played, so that all three notes have their own more natural sounding decay.
}

}

void myControlChange(byte channel, byte number, byte value) {
  MIDI1.sendControlChange(number, value, channel);
}

void setup(void)
{
  
  Serial.begin(115200);
  Serial.println("OPERATIONAL!");

  b01Toggle.attach(B01_PIN, INPUT_PULLUP); // USE EXTERNAL PULL-UP
  b02Toggle.attach(B02_PIN, INPUT_PULLUP); // USE EXTERNAL PULL-UP  
  // DEBOUNCE INTERVAL IN MILLISECONDS
  b01Toggle.interval(5); 
  b02Toggle.interval(5);

  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  b01Toggle.setPressedState(0);
  b02Toggle.setPressedState(0); 

  
  r01.setPosition(12 / r01STEPS); // start with the value of 0. - First in the menu.
  r02.setPosition(0 / r02STEPS);
  // printInit();

  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  //MIDI.begin(MIDI_CHANNEL_OMNI);  // USB MIDI

  MIDI1.turnThruOff();
  MIDI2.turnThruOff();
  //MIDI.turnThruOff();  // USB MIDI

  MIDI1.setHandleNoteOn(myNoteOn);
  MIDI2.setHandleNoteOn(myNoteOn);
  //MIDI.setHandleNoteOn(myNoteOn);  // USB MIDI

  MIDI1.setHandleNoteOff(myNoteOff);
  MIDI2.setHandleNoteOff(myNoteOff);
  //MIDI.setHandleNoteOff(myNoteOff);  // USB MIDI

  MIDI1.setHandleControlChange(myControlChange);
  MIDI2.setHandleControlChange(myControlChange);
  //MIDI.setHandleControlChange(myControlChange);  // USB MIDI

  u8x8.begin();
  u8x8.clear();
u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
u8x8.draw2x2String(4, 0, "AUTO");
u8x8.inverse();
u8x8.draw2x2String(4, 2, "BASS");
u8x8.noInverse();
u8x8.drawString(5, 4, "M i n i");
u8x8.drawString(6, 6, "By");
u8x8.drawString(1, 7, "Jonathan Zeppa");
delay(2000);
u8x8.clear();

  u8x8.setFont(u8x8_font_8x13_1x2_f);
  u8x8.setCursor(0, 0);
  u8x8.print(Key1[0]);
  u8x8.setCursor(4, 0);
  u8x8.print(Mode2[0]);
  //u8x8.setFont(u8x8_font_8x13B_1x2_r);
  //u8x8.setFont(u8x8_font_profont29_2x3_f);  //Viable
  //u8x8.setFont(u8x8_font_inb33_3x6_r);
  u8x8.setFont(u8x8_font_inr21_2x4_f);
  u8x8.setCursor(0, 2);
  u8x8.printf("  ");
  //u8x8.printf("%d", show);
  // u8x8.drawString(0, 2, "%d", show); This does NOT work
  //u8x8.setFont(u8x8_font_8x13_1x2_f);
  //u8x8.drawString(5, 2, "4");
  u8x8.setFont(u8x8_font_inr21_2x4_f);
  u8x8.drawString(8, 2, "    "); // Insert AutoBass Logo?
  u8x8.setFont(u8x8_font_8x13_1x2_r);
  }


String notes[12]  = {" ","C"," ","D"," "," ","F"," ","G"," ","A"," "};
String sharps[12] = {"C ","#","D ","#","E ","F ","#","G ","#","A ","#","B "};
void printNote (uint8_t displayNote) { // removed uint8_t note
  // MIDI notes go from A0 = 21 up to G9 = 127
  u8x8.setFont(u8x8_font_inr21_2x4_f);
  u8x8.setCursor(0, 2);
  //u8x8.printf("  ");
  u8x8.setCursor(0, 2);
  u8x8.print(notes[displayNote%12]);
  u8x8.print(sharps[displayNote%12]);
  u8x8.setFont(u8x8_font_8x13_1x2_f);
  u8x8.setCursor(5, 2);
  u8x8.print((displayNote/12)-1,DEC); 
  u8x8.print(" "); // is this needed?
  u8x8.setFont(u8x8_font_inr21_2x4_f);
  u8x8.setCursor(8, 2); 

if (b01 == 0){
 
  u8x8.print(chordName[chord]);}   // cursor position, font size
  else {
    u8x8.print("    ");
    }

}



void loop() {
  
//MIDI1.read();
b01Toggle.update();
b02Toggle.update();

int channel;
int noteNUM;
//int note02;
 
  bool activity = false; // Do I need this?

//  if (MIDI1.read()) {
//    // get a MIDI IN1 (Serial) message
//    byte type = MIDI1.getType();
//    byte channel = MIDI1.getChannel();
//    byte noteNUM = MIDI1.getData1();
//    byte data2 = MIDI1.getData2();
//    Serial.println(noteNUM);
//        if (channel == 1) { printNote (root+transpose); }
//     //default: // Otherwise it is a message we aren't interested in
//        //break;

  if (MIDI1.read()) {
    //byte type = MIDI1.getType();  
    switch (MIDI1.getType()) {
      case midi::NoteOn:
      channel = MIDI1.getChannel();
      noteNUM = MIDI1.getData1();
      //note02 = MIDI1.getData2();
      if (channel == 1) {
        printNote(root+transpose);
        Serial.println(noteNUM);
        } 
      
//      if (channel == 10) {  
//        Serial.print("Note: "); //Data-01
//        Serial.print(noteNUM);
//        Serial.print(" Channel: "); //channel
//        Serial.println(channel);
//        break;
//        }
        break;
      default: // Otherwise it is a message we aren't interested in
        break;
    }
 
    // forward the message to USB MIDI virtual cable #0
//        if (type != midi::SystemExclusive) {
//      // Normal messages, simply give the data to the usbMIDI1.send()
//      //MIDI1.send(type, data1, data2, channel, 0);
      //MIDI1.send(type, noteNUM, note02, channel);
//      Serial.println("001");
//      Serial.print("MIDI-01: ");
//      Serial.println(noteNUM);
//      Serial.print("Channel: ");
//      Serial.println(channel);
//      //MIDI2.send(type, noteNUM, note02, channel);  
//   } 
//
//else {
//      // SysEx messages are special.  The message length is given in data1 & data2
//      unsigned int SysExLength = noteNUM + note02 * 256;
//      //MIDI1.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 0);
//      //MIDI1.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
//      //MIDI2.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);    
//}
    activity = true;
  }

  if (MIDI2.read()) {
    // get a MIDI IN2 (Serial) message
    byte type = MIDI2.getType();
    byte channel = MIDI2.getChannel();
    byte data1 = MIDI2.getData1(); // Note
    byte data2 = MIDI2.getData2(); // Velocity

//if  (channel == 2) {
//MIDI2.sendNoteOn(data1, data2, channel);



    // forward the message to USB MIDI virtual cable #1
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI1.send()
      //MIDI1.send(type, data1, data2, channel, 1);
      //MIDI1.send(type, data1, data2, channel);
      //Serial.print("data1 "); Serial.println (data1);
      //Serial.print("data2 ");Serial.println (data2);
      //Serial.print("channel ");Serial.println (channel);
      //MIDI2.send(type, data1, data2, channel);
 // END Channel 10
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      //MIDI1.sendSysEx(SysExLength, MIDI2.getSysExArray(), true, 1);
      MIDI1.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      //MIDI2.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      }

              
    activity = true;
  }

// Encoder Section:

r01.tick();  // Key

  // get the current physical position and calc the logical position
  int r01newPos = r01.getPosition() * r01STEPS;

  if (r01newPos < r01MIN) {
    r01.setPosition(r01MIN / r01STEPS);
    r01newPos = r01MIN;

  } else if (r01newPos > r01MAX) {
    r01.setPosition(r01MAX / r01STEPS);
    r01newPos = r01MAX;
  } // if

  if (r01lastPos != r01newPos) {
    //Serial.print(Key1[newPos]);
    //Serial.println();
    u8x8.setFont(u8x8_font_8x13_1x2_f);
    u8x8.setCursor(0, 0);
    u8x8.print(Key1[r01newPos]); //println?
    //u8x8.print(" ");
    r01lastPos = r01newPos;
    transpose = (keyNUM[r01newPos]);
  } // if

r02.tick();   // Mode

  int r02newPos = r02.getPosition() * r02STEPS;
  if (r02newPos < r02MIN) {
    r02.setPosition(r02MIN / r02STEPS);
    r02newPos = r02MIN;
  } else if (r02newPos > r02MAX) {
    r02.setPosition(r02MAX / r02STEPS);
    r02newPos = r02MAX;
  } // if
  if (r02lastPos != r02newPos) {
    //Serial.print(Key1[newPos]);
    //Serial.println();
    u8x8.setFont(u8x8_font_8x13_1x2_f);
    u8x8.setCursor(4, 0);
    u8x8.print(Mode2[r02newPos]); //println?
    r02lastPos = r02newPos;
    modeVar = (r02newPos);  // modeSelect2
  } // if


// Buttons:
  if ( b01Toggle.pressed() ) {
    
    // TOGGLE THE LED STATE : 
    b01 = !b01; // SET ledState TO THE OPPOSITE OF ledState
    u8x8.setFont(u8x8_font_8x13_1x2_r);
    u8x8.setCursor(2, 6);
    //digitalWrite(LED_PIN,ledState); // WRITE THE NEW ledState
    if (b01 == 1){
      Serial.println("Chord Off!");
      Serial.println(b01);
      u8x8.print("      ");
    }
    else {
      Serial.println("Chord On!");
      Serial.println(b01);
      u8x8.print("Chord ");
    }
} // b01Toggle

  if ( b02Toggle.pressed() ) {
    
    // TOGGLE THE LED STATE : 
    b02 = !b02; // SET ledState TO THE OPPOSITE OF ledState
    u8x8.setFont(u8x8_font_8x13_1x2_r);
    u8x8.setCursor(10, 6);
    //digitalWrite(LED_PIN,ledState); // WRITE THE NEW ledState
    if (b02 == 1){
      Serial.println("HOLD Off!");
      Serial.println(b02);
      u8x8.print("     ");
    MIDI1.sendControlChange(123, 0, 1);
    MIDI1.sendControlChange(120, 0, 1);
    //MIDI.sendControlChange(123, 0, 1);
    //MIDI.sendControlChange(120, 0, 1);

    delay(50);
    MIDI1.sendControlChange(123, 0, 2);
    MIDI1.sendControlChange(120, 0, 2);
    //MIDI.sendControlChange(123, 0, 1);
    //MIDI.sendControlChange(120, 0, 1);
    Serial.println("PANIC!");
    }
    else {
      Serial.println("HOLD On!");
      Serial.println(b02);
      u8x8.print("Hold ");
    MIDI1.sendControlChange(123, 0, 1);
    MIDI1.sendControlChange(120, 0, 1);
    //MIDI.sendControlChange(123, 0, 1);
    //MIDI.sendControlChange(120, 0, 1);
    delay(50);
    MIDI1.sendControlChange(123, 0, 2);
    MIDI1.sendControlChange(120, 0, 2); 
    //MIDI.sendControlChange(123, 0, 1);
    //MIDI.sendControlChange(120, 0, 1);

    Serial.println("PANIC!");
    }
} // b01Toggle

} //end loop