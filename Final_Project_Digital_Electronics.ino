#include <SerialFlash.h>

int switchPin = 23;
int ledPinArray[12] = {22, 21, 20, 19, 8, 9, 11, 12, 26, 27, 29, 30};
int playButton = 33;
int stopButton = 34;
int Time = 0;
int majorMinorSwitch = 38;

int octaveNext = 24;   //button pin for next octave
int octavePrev = 32;    //buttonPin for prev octave
int octaveNextLed = 28;
int octavePrevLed = 7;
boolean lastOctaveState = LOW;
boolean octaveState = LOW;
unsigned long lastOctaveStateChange;

boolean lastOctaveState1 = LOW;     //variable for octave down button
boolean octaveState1 = LOW;
unsigned long lastOctaveStateChange1;

int counter = 0;

int currentStep = 0;
unsigned long lastStepTime = 0;

boolean lastButtonState = LOW;
boolean buttonState = LOW;
boolean on = false;


int majorThird = 4;          // Major 3rd interval in MIDI
int minorThird = 3;          // minor 3rd interval in MIDI
int perfectFifth = 7;        // Perfect 5th interval in MIDI
int perfectFourth = 5;       // Perfect 4th interval in MIDI
int octave = 12;             // Octave Interval in MIDI
int majorSixth = 9;          // Major Sixth interval in MIDI
int minorSixth = 8;          // Major Sixth interval in MIDI


int keyNote = 48;

int level1MajorChords[8][4];
int level1MinorChords[8][4];

int level2MajorChords[12][4];
int level2MinorChords[12][4];

int level3MajorChords[12][4];
int level3MinorChords[12][4];

int iChords[4][4] = {
  {0,  majorThird,  majorThird + minorThird,  octave},// Major I chord
  {majorThird,  majorThird + minorThird,  octave, octave + perfectFifth},// Major I 6 chord
  {0,  minorThird,  majorThird + minorThird,  octave},// Minor i chord
  {minorThird,  majorThird + minorThird,  octave, octave + perfectFifth}, // Minor i 6 chord
};

int preDominantChords[4][4] = {
  {perfectFourth, perfectFourth + majorThird, octave + 2, octave + perfectFourth}, //minor ii
  {perfectFourth, perfectFourth + majorThird, perfectFourth + majorThird + minorThird, perfectFourth + octave}, // major IV
  {perfectFourth, perfectFourth + minorThird, octave + 2, octave + perfectFourth}, //dim  ii
  {perfectFourth, perfectFourth + minorThird, perfectFourth + majorThird + minorThird, perfectFourth + octave} // minor iv
};

int iiChords[4][4] {

  {2, 5, 9, 14},
  {perfectFourth, perfectFourth + majorThird, perfectFourth + majorThird + minorThird, perfectFourth + octave},
  {2, 5, 8, 14},
  {perfectFourth, perfectFourth + minorThird, perfectFourth + majorThird + minorThird, perfectFourth + octave}
};


int viChords[2][4] = {
  {majorSixth - octave, 0, majorThird , majorSixth}, //minor
  {minorSixth - octave, 0, minorThird , minorSixth} // flat/major
};

int viiChords [3][4] = {
  { -1, 2, 5, 8}, //diminished 7 chord
  {2, 5, 8, octave - 1}, //65
  {5 - octave , 8 - octave, -1,  2} //43
};


int secondaryDominantChords [9][4] = {
  {perfectFifth - octave - 1, -3 , 0, 2 }, // V / V //secondary dominant chords of dominant chords and predominant chords
  {perfectFifth - 1 , octave - 3, octave, octave + 2},  //V/V7
  {perfectFifth - 1 , octave - 3, octave, octave + 2},    //V/V65
  {0, 2 , octave - 3, octave - 1 },   //V/V43
  {2, octave - 3, octave - 1 , 0},    //V/42
  {majorThird, majorSixth, octave + 1, octave + perfectFifth}, //V/ii6
  {majorThird, perfectFifth, octave - 2, octave },   //V/IV
  {majorThird, majorSixth, octave + 1, octave + perfectFifth},  //V/iidim
  {majorThird, perfectFifth, octave - 2, octave } //V/iv

};


int vChords[7][4] = {
  {perfectFifth - octave , -1, 2, perfectFifth},  //triad
  {perfectFifth, perfectFifth + majorThird, perfectFifth + majorThird + minorThird, perfectFifth + octave - 2}, // V7 chord
  { - 1, 2, perfectFourth, perfectFifth}, //65 (1st inversion )
  {2, perfectFourth, perfectFifth, octave - 1},  //43 (2nd inversion)
  {perfectFourth , perfectFifth, octave - 1, octave + 2} ,//42 (3rd inversion)
  {perfectFifth - octave , 0 , majorThird, perfectFifth}, //passing 64 major
  {perfectFifth - octave , 0 , minorThird, perfectFifth} //passing 64 minor
};

int morePreDominantChords[4][4] {
  { perfectFourth - octave, minorSixth - octave , 1, perfectFourth}, //Neapolitan Chord (flat II6)
  { perfectFourth, perfectFourth + minorThird, octave + 1, octave + perfectFourth}, //Neapolitan in another position
  { perfectFifth + 1 - octave, 0, minorThird, perfectFifth - 1},    //german Augmented 6 Chord
  { perfectFifth + 1 - octave, 0, 2, perfectFifth - 1},    //French Augmented 6 Chord
};

void setup() {
  //Final Project Digital Electronics
  Serial.begin(9600);
  Serial1.begin(9600);
  setBrightness(255);

  for (int i = 0; i < 12; i++) {
    pinMode(ledPinArray[i], OUTPUT);

    //setup pinmodes
  }
  pinMode(playButton, INPUT);
  pinMode(switchPin, INPUT);
  pinMode(majorMinorSwitch, INPUT);
  pinMode(octaveNext, INPUT);
  pinMode(octavePrev, INPUT);
  pinMode(octaveNextLed, OUTPUT);
  pinMode(octavePrevLed, OUTPUT);
}

void loop() {
  Time = analogRead(A12);

  int  keyPot = analogRead(A17);
  int mappedKeyPot = map(keyPot, 1, 1023, 0 , 12);
  keyNote = mappedKeyPot + 48;
  checkOctaveNext();
  checkOctavePrev();
  keyNote = keyNote + (counter * 12);

  //  Serial.println(counter);
  int levelPot = analogRead(A1);
  int mappedLevelPot = map(levelPot, 1, 1023, 1, 3);
  //  Serial.println(levelPot);
  //  Serial.println(mappedLevelPot);

  displayKey(keyNote, mappedLevelPot);

  updateLevel1Chords(keyNote);
  updateLevel2MajorChords(keyNote);
  updateLevel2MinorChords(keyNote);
  updateLevel3MajorChords(keyNote);
  updateLevel3MinorChords(keyNote);
  delay(10);
  checkPlayButton();
  if (mappedLevelPot == 1) {
    if (digitalRead(majorMinorSwitch) == LOW) {
      if (digitalRead(switchPin) == LOW)
        level1MajorChordMode();
      else level1MajorArpeggioMode();
    }
    else {
      if (digitalRead(switchPin) == LOW)
        level1MinorChordMode();
      else level1MinorArpeggioMode();
    }
  }
  else  if (mappedLevelPot == 2) {
    if (digitalRead(majorMinorSwitch) == LOW) {
      if (digitalRead(switchPin) == LOW)
        level2MajorChordMode();
      else level2MajorArpeggioMode();
    }
    else {
      if (digitalRead(switchPin) == LOW)
        level2MinorChordMode();
      else level2MinorArpeggioMode();
    }
  }
  else {
    if (digitalRead(majorMinorSwitch) == LOW) {
      if (digitalRead(switchPin) == LOW)
        level3MajorChordMode();
      else level3MajorArpeggioMode();
    }
    else {
      if (digitalRead(switchPin) == LOW)
        level3MinorChordMode();
      else level3MinorArpeggioMode();
    }
  }
  Serial.println(counter);
}





void updateLevel1Chords(int keyNote) {
  int c = random (6);
  int lastChord = random (3);
  for (int i = 0; i < 8; i ++) {
    int a = random(5);
    int b =  random(2);
    for (int j = 0 ; j < 4 ; j ++) {
      if (i == 0) {
        level1MajorChords[i][j] = iChords[b][j] + keyNote;  //first chord will always be a 1 chord
      }
      if (i == 1) {
        level1MajorChords[i][j] = vChords[a][j] + keyNote;   //second chord is either an inversion of a V chord or a IV chord or an inversion of vii Chord
      }
      if (i == 2) {
        level1MajorChords[i][j] = iChords[b][j] + keyNote;  //Third chord is always a I 6 chord
      }
      if (i == 3) {
        level1MajorChords[i][j] = iiChords[b][j] + keyNote;
      }
      if (i == 4) {
        level1MajorChords[i][j] = iiChords[0][j] + keyNote; //we are playing 2 predominant chords, the 1st could be any of the 2, the second would always be a ii6 chord
      }
      if (i == 5) {
        level1MajorChords[i][j] = vChords[c][j] + keyNote;
      }
      if (i == 6) {
        if (c == 5) {
          level1MajorChords[i][j] = vChords[0][j] + keyNote;  //v chord after passing 64 to make a cadential 64
        }
        else if (lastChord != 2) {
          level1MajorChords[i][j] = iChords[lastChord][j] + keyNote;
        }
        else if (lastChord == 2) {
          level1MajorChords[i][j] = viChords[0][j] + keyNote;
        }
      } //if the chord is  V TRIAD THEN WE WILL USE A CADENTIAL 64. IF NOT WE WILL MOVE ON TO THE I CHORD

      if (i == 7) {
        level1MajorChords[i][j] = iChords[b][j] + keyNote;
      }
    }

  }


  if (c == 5) {
    c = 6; //necessary because passing 64 major cannot be in minor (variable 'c' is used to choose a type of V chord at the last cadence)
  }

  for (int i = 0; i < 8; i ++) {
    int a = random(5);
    int b =  random(2);
    for (int j = 0 ; j < 4 ; j ++) {
      if (i == 0) {
        level1MinorChords[i][j] = iChords[b + 2][j] + keyNote; //first chord will always be a 1 chord
      }
      if (i == 1) {
        level1MinorChords[i][j] = vChords[a][j] + keyNote;   //second chord is either an inversion of a V chord or a IV chord or an inversion of vii Chord
      }
      if (i == 2) {
        level1MinorChords[i][j] = iChords[b + 2][j] + keyNote; //Third chord is always a I 6 chord
      }
      if (i == 3) {
        level1MinorChords[i][j] = iiChords[b + 2][j] + keyNote; //
      }
      if (i == 4) {
        level1MinorChords[i][j] = iiChords[2][j] + keyNote; //we are playing 2 predominant chords, the 1st could be any of the 2, the second would always be a iidim6 chord
      }
      if (i == 5) {
        level1MinorChords[i][j] = vChords[c][j] + keyNote;
      }
      if (i == 6) {
        if (c == 5) {
          level1MinorChords[i][j] = vChords[0][j] + keyNote;  //v chord after passing 64 to make a cadential 64
        }
        else if (lastChord != 2) {
          level1MinorChords[i][j] = iChords[lastChord + 2][j] + keyNote;
        }
        else if (lastChord == 2) {
          level1MinorChords[i][j] = viChords[1][j] + keyNote;
        }
      } //if the chord is  V TRIAD THEN WE WILL USE A CADENTIAL 64. IF NOT WE WILL MOVE ON TO THE I CHORD

      if (i == 7) {
        level1MinorChords[i][j] = iChords[b + 2][j] + keyNote;
      }


    }
  }

}

void updateLevel2MajorChords(int keyNote) {
  int flag1 = 0;
  int flag2 = 0;
  int flag3 = 0;
  int g = random(5);
  int a = random(2);
  for (int i = 0; i < 12; i++) {
    int c = random(10);
    int b =  random(2);
    int d = random(2);
    int e = random(8);
    int f = random (5);
    int h = random(4);
    int k = random(3);
    int l = random(3);
    int secondary = random(4);    //decides if the next chord is a secondary dominant or not
    for (int j = 0; j < 4; j++) {
      if (i == 0) {                                        //first chord will be a type of 1 chord
        level2MajorChords[i][j] = iChords[b][j] + keyNote;
      }
      if (i == 1) {  //in level 2 the second chord can either be a type of v chord or a type of vii chord or a predominant
        if (c < 5) {
          level2MajorChords[i][j] = vChords[c][j] + keyNote;
        }

        else if (c < 7) {
          level2MajorChords[i][j] = viiChords[c - 5][j] + keyNote;
          flag1 = 1;
        }
        else {
          level2MajorChords[i][j] = iiChords[b][j] + keyNote;
        }
      }
      if (i == 2) {
        if (b == 0) {
          level2MajorChords[i][j] = secondaryDominantChords[a + 5][j] + keyNote;
        }
        else if (b == 1) {
          level2MajorChords[i][j] = iChords[a][j] + keyNote;
        }
      }
      if (i == 3) {
        level2MajorChords[i][j] = iiChords[a][j] + keyNote;
      }
      if (i == 4) {
        if (e < 5) {
          level2MajorChords[i][j] = secondaryDominantChords[2][j] + keyNote;
        }
        else if (e < 7) {
          level2MajorChords[i][j] = iiChords[0][j] + keyNote;
        }
        else
          level2MajorChords[i][j] = vChords[5][j] + keyNote; //passing64
        flag1 = 1;
      }
      if (i == 5) {
        if (flag1 == 1) {
          level2MajorChords[i][j] = vChords[0][j] + keyNote;
        }
        else if (b == 0 ) {
          level2MajorChords[i][j] = iChords[d][j] + keyNote;
        }
        else if (b == 1) {
          level2MajorChords[i][j] = viChords[0][j] + keyNote;
        }
      }
      if (i == 6) {
        level2MajorChords[i][j] = viChords[0][j] + keyNote;
      }
      if (i == 7) {
        if (h < 2) {
          level2MajorChords[i][j] = secondaryDominantChords[h + 5][j] + keyNote;

        }
        else if (h < 4) {
          level2MajorChords[i][j] = iiChords[h - 2][j] + keyNote;
        }
        flag2 = h;
      }
      if (i == 8) {
        if (flag2 < 2) {
          level2MajorChords[i][j] = iiChords[b][j] + keyNote;
        }
        else {
          if (k == 0) {
            level2MajorChords[i][j] = iiChords[0][j] + keyNote;
          }
          else {
            level2MajorChords[i][j] = secondaryDominantChords[f][j] + keyNote;
            flag3 = 1;
          }
        }
      }
      if (i == 9) {
        if (flag3 == 0) {
          if (k == 0) {
            level2MajorChords[i][j] = secondaryDominantChords[f][j] + keyNote;
          }
          else if (k == 1) {
            level2MajorChords[i][j] = vChords[f + 1][j] + keyNote;
          }
          else if (k == 2) {
            level2MajorChords[i][j] = viiChords[l][j] + keyNote;
          }
        }
        else {
          level2MajorChords[i][j] = vChords[5][j] + keyNote;
        }
      }
      if (i == 10) {
        level2MajorChords[i][j] = vChords[b][j] + keyNote;
      }
      if (i == 11) {
        level2MajorChords[i][j] = iChords[0][j] + keyNote;
      }

    }


  }
}


void updateLevel2MinorChords(int keyNote) {
  int flag1 = 0;
  int flag2 = 0;
  int flag3 = 0;
  int g = random(5);
  int a = random(2);
  for (int i = 0; i < 12; i++) {
    int c = random(10);
    int b =  random(2);
    int d = random(2);
    int e = random(8);
    int f = random (5);
    int h = random(4);
    int k = random(3);
    int l = random(3);
    int secondary = random(4);    //decides if the next chord is a secondary dominant or not
    for (int j = 0; j < 4; j++) {
      if (i == 0) {                                        //first chord will be a type of 1 chord
        level2MinorChords[i][j] = iChords[b + 2][j] + keyNote;
      }
      if (i == 1) {  //in level 2 the second chord can either be a type of v chord or a type of vii chord or a predominant
        if (c < 5) {
          level2MinorChords[i][j] = vChords[c][j] + keyNote;
        }

        else if (c < 7) {
          level2MinorChords[i][j] = viiChords[c - 5][j] + keyNote;
          flag1 = 1;
        }
        else {
          level2MinorChords[i][j] = iiChords[b + 2][j] + keyNote;
        }
      }
      if (i == 2) {
        if (b == 0) {
          level2MinorChords[i][j] = secondaryDominantChords[a + 5][j] + keyNote;
        }
        else if (b == 1) {
          level2MinorChords[i][j] = iChords[a + 2][j] + keyNote;
        }
      }
      if (i == 3) {
        level2MinorChords[i][j] = iiChords[a + 2][j] + keyNote;
      }
      if (i == 4) {
        if (e < 5) {
          level2MinorChords[i][j] = secondaryDominantChords[4][j] + keyNote;
        }
        else if (e < 7) {
          level2MinorChords[i][j] = iiChords[2][j] + keyNote;
        }
        else
          level2MinorChords[i][j] = vChords[6][j] + keyNote; //passing64
        flag1 = 1;
      }
      if (i == 5) {
        if (flag1 == 1) {
          level2MinorChords[i][j] = vChords[0][j] + keyNote;
        }
        else if (b == 0 ) {
          level2MinorChords[i][j] = iChords[d][j] + keyNote;
        }
        else if (b == 1) {
          level2MinorChords[i][j] = viChords[1][j] + keyNote;
        }
      }
      if (i == 6) {
        level2MinorChords[i][j] = viChords[1][j] + keyNote;
      }
      if (i == 7) {
        if (h < 2) {
          level2MinorChords[i][j] = secondaryDominantChords[h + 5][j] + keyNote;

        }
        else if (h < 4) {
          level2MinorChords[i][j] = iiChords[h][j] + keyNote;
        }
        flag2 = h;
      }
      if (i == 8) {
        if (flag2 < 2) {
          level2MinorChords[i][j] = iiChords[b + 2][j] + keyNote;
        }
        else {
          if (k == 0) {
            level2MinorChords[i][j] = iiChords[2][j] + keyNote;
          }
          else {
            level2MinorChords[i][j] = secondaryDominantChords[f][j] + keyNote;
            flag3 = 1;
          }
        }
      }
      if (i == 9) {
        if (flag3 == 0) {
          if (k == 0) {
            level2MinorChords[i][j] = secondaryDominantChords[f][j] + keyNote;
          }
          else if (k == 1) {
            level2MinorChords[i][j] = vChords[f + 1][j] + keyNote;
          }
          else if (k == 2) {
            level2MinorChords[i][j] = viiChords[l][j] + keyNote;
          }
        }
        else {
          level2MinorChords[i][j] = vChords[6][j] + keyNote;
        }
      }
      if (i == 10) {
        level2MinorChords[i][j] = vChords[b][j] + keyNote;     //V triad or V 7
      }
      if (i == 11) {
        level2MinorChords[i][j] = iChords[2][j] + keyNote;    //i chord
      }

    }


  }
}



void updateLevel3MajorChords(int keyNote) {
  int flag1 = 0;
  int flag2 = 0;
  int flag3 = 0;
  int g = random(5);
  int a = random(2);
  int m = random(4);
  for (int i = 0; i < 12; i++) {
    int c = random(10);
    int b =  random(2);
    int d = random(2);
    int e = random(8);
    int f = random (5);
    int h = random(4);
    int k = random(3);
    int l = random(3);
    int secondary = random(4);    //decides if the next chord is a secondary dominant or not
    for (int j = 0; j < 4; j++) {
      if (i == 0) {                                        //first chord will be a type of 1 chord
        level3MajorChords[i][j] = iChords[b][j] + keyNote;
      }
      if (i == 1) {  //in level 2 the second chord can either be a type of v chord or a type of vii chord or a predominant
        if (c < 5) {
          level3MajorChords[i][j] = vChords[c][j] + keyNote;
        }

        else if (c < 7) {
          level3MajorChords[i][j] = viiChords[c - 5][j] + keyNote;
          flag1 = 1;
        }
        else {
          level3MajorChords[i][j] = iiChords[h][j] + keyNote;   //modal Mixture
        }
      }
      if (i == 2) {
        if (b == 0) {
          level3MajorChords[i][j] = secondaryDominantChords[a + 5][j] + keyNote;
        }
        else if (b == 1) {
          level3MajorChords[i][j] = iChords[h][j] + keyNote;
        }
      }
      if (i == 3) {
        level3MajorChords[i][j] = iiChords[m][j] + keyNote;
      }
      if (i == 4) {
        if (e < 5) {
          level3MajorChords[i][j] = secondaryDominantChords[2][j] + keyNote;
        }
        else if (e < 7) {
          level3MajorChords[i][j] = iiChords[h][j] + keyNote;
        }
        else
          level3MajorChords[i][j] = vChords[5][j] + keyNote; //passing64
        flag1 = 1;
      }
      if (i == 5) {
        if (flag1 == 1) {
          level3MajorChords[i][j] = vChords[0][j] + keyNote;
        }
        else if (b == 0 ) {
          level3MajorChords[i][j] = iChords[d][j] + keyNote;
        }
        else if (b == 1) {
          level3MajorChords[i][j] = viChords[d][j] + keyNote;
        }
      }
      if (i == 6) {
        level3MajorChords[i][j] = viChords[b][j] + keyNote;
      }
      if (i == 7) {
        if (h < 2) {
          level3MajorChords[i][j] = secondaryDominantChords[h + 5][j] + keyNote;

        }
        else if (h < 4) {
          level3MajorChords[i][j] = iiChords[h][j] + keyNote;
        }
        flag2 = h;
      }
      if (i == 8) {
        if (flag2 < 4) {
          level3MajorChords[i][j] = iiChords[h][j] + keyNote;
        }
        else {
          if (k == 0) {
            level3MajorChords[i][j] = iiChords[h][j] + keyNote;
          }
          else {
            level3MajorChords[i][j] = secondaryDominantChords[f][j] + keyNote;
            flag3 = 1;
          }
        }
      }
      if (i == 9) {
        if (flag3 == 0) {
          if (k == 0) {
            level3MajorChords[i][j] = secondaryDominantChords[f][j] + keyNote;
          }
          else if (k == 1) {
            level3MajorChords[i][j] = vChords[f + 1][j] + keyNote;
          }
          else if (k == 2) {
            level3MajorChords[i][j] = viiChords[l][j] + keyNote;
          }
        }
        else {
          level3MajorChords[i][j] = vChords[5][j] + keyNote;
        }
      }
      if (i == 10) {
        level3MajorChords[i][j] = vChords[b][j] + keyNote;
      }
      if (i == 11) {
        level3MajorChords[i][j] = iChords[0][j] + keyNote;
      }

    }


  }
}

void updateLevel3MinorChords(int keyNote) {
  int flag1 = 0;
  int flag2 = 0;
  int flag3 = 0;
  int g = random(5);
  int a = random(2);
  for (int i = 0; i < 12; i++) {
    int c = random(10);
    int b =  random(2);
    int d = random(2);
    int e = random(8);
    int f = random (5);
    int h = random(4);
    int k = random(3);
    int l = random(3);
    int m = random(4);
    int secondary = random(4);    //decides if the next chord is a secondary dominant or not
    for (int j = 0; j < 4; j++) {
      if (i == 0) {                                        //first chord will be a type of 1 chord
        level3MinorChords[i][j] = iChords[b + 2][j] + keyNote;
      }
      if (i == 1) {  //in level 2 the second chord can either be a type of v chord or a type of vii chord or a predominant
        if (c < 5) {
          level3MinorChords[i][j] = vChords[c][j] + keyNote;
        }

        else if (c < 7) {
          level3MinorChords[i][j] = viiChords[c - 5][j] + keyNote;
          flag1 = 1;
        }
        else {
          level3MinorChords[i][j] = iiChords[h][j] + keyNote;
        }
      }
      if (i == 2) {
        if (b == 0) {
          level3MinorChords[i][j] = secondaryDominantChords[a + 5][j] + keyNote;
        }
        else if (b == 1) {
          level3MinorChords[i][j] = iChords[h][j] + keyNote;
        }
      }
      if (i == 3) {
        level3MinorChords[i][j] = iiChords[h][j] + keyNote;
      }
      if (i == 4) {
        if (e < 5) {
          level3MinorChords[i][j] = secondaryDominantChords[4][j] + keyNote;
        }
        else if (e < 7) {
          level3MinorChords[i][j] = iiChords[h][j] + keyNote;
        }
        else
          level3MinorChords[i][j] = vChords[6][j] + keyNote; //passing64
        flag1 = 1;
      }
      if (i == 5) {
        if (flag1 == 1) {
          level3MinorChords[i][j] = vChords[0][j] + keyNote;
        }
        else if (b == 0 ) {
          level3MinorChords[i][j] = iChords[d][j] + keyNote;
        }
        else if (b == 1) {
          level3MinorChords[i][j] = viChords[1][j] + keyNote;
        }
      }
      if (i == 6) {
        level3MinorChords[i][j] = viChords[1][j] + keyNote;
      }
      if (i == 7) {
        if (h < 2) {
          level3MinorChords[i][j] = secondaryDominantChords[h + 5][j] + keyNote;

        }
        else if (h < 4) {
          level3MinorChords[i][j] = iiChords[h][j] + keyNote;
        }
        flag2 = h;
      }
      if (i == 8) {
        if (flag2 < 2) {
          level3MinorChords[i][j] = iiChords[b + 2][j] + keyNote;
        }
        else {
          if (k == 0) {
            level3MinorChords[i][j] = iiChords[2][j] + keyNote;
          }
          else {
            level3MinorChords[i][j] = secondaryDominantChords[f][j] + keyNote;
            flag3 = 1;
          }
        }
      }
      if (i == 9) {
        if (flag3 == 0) {
          if (h == 0) {
            level3MinorChords[i][j] = secondaryDominantChords[f][j] + keyNote;
          }
          else if (h == 1) {
            level3MinorChords[i][j] = vChords[f + 1][j] + keyNote;
          }
          else if (h == 2) {
            level3MinorChords[i][j] = viiChords[l][j] + keyNote;
          }
          else if (h == 3) {
            level3MinorChords[i][j] = morePreDominantChords[m][j] + keyNote;
          }
        }
        else {
          level3MinorChords[i][j] = vChords[6][j] + keyNote;
        }
      }
      if (i == 10) {
        level3MinorChords[i][j] = vChords[b][j] + keyNote;     //V triad or V 7
      }
      if (i == 11) {
        level3MinorChords[i][j] = iChords[h][j] + keyNote;    //i chord
      }

    }


  }
}





void checkPlayButton() {

  //if (millis() > lastStepTime + 100) {
  lastButtonState = buttonState;
  buttonState = digitalRead(playButton);
  Serial.println(on);
  lastStepTime = millis();
  // }

  if (buttonState == HIGH && lastButtonState == LOW) {
    lastStepTime = millis();
    if (on == false) {
      on = true;
    } else if (on == true) {
      lastStepTime = millis();
      on = false;
    }
  }
}


void level1MajorArpeggioMode() {
  if (on == true) {

    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 4; j++) {
        delay(Time);
        usbMIDI.sendNoteOn(level1MajorChords[i][j], 100, 1);
        digitalWrite(ledPinArray[j], HIGH);
        delay(Time);
        usbMIDI.sendNoteOff(level1MajorChords[i][j], 0, 1);
        digitalWrite(ledPinArray[j], LOW);
      }
      for (int j = 2; j > 0; j--) {
        delay(Time);
        digitalWrite(ledPinArray[j], HIGH);
        usbMIDI.sendNoteOn(level1MajorChords[i][j], 100, 1);
        delay(Time);
        digitalWrite(ledPinArray[j], LOW);
        usbMIDI.sendNoteOff(level1MajorChords[i][j], 0, 1);

      }
    }
  } else  {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}


void level1MinorArpeggioMode() {
  if (on == true) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 4; j++) {
        delay(Time);
        usbMIDI.sendNoteOn(level1MinorChords[i][j], 100, 1);
        digitalWrite(ledPinArray[j], HIGH);
        delay(Time);
        usbMIDI.sendNoteOff(level1MinorChords[i][j], 0, 1);
        digitalWrite(ledPinArray[j], LOW);
      }
      for (int j = 2; j > 0; j--) {
        delay(Time);
        digitalWrite(ledPinArray[j], HIGH);
        usbMIDI.sendNoteOn(level1MinorChords[i][j], 100, 1);
        delay(Time);
        digitalWrite(ledPinArray[j], LOW);
        usbMIDI.sendNoteOff(level1MinorChords[i][j], 0, 1);
      }
    }
  } else  {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}

void level1MajorChordMode() {
  if (on == true) {
    for (int i = 0; i < 8; i++) {
      checkPlayButton();
      for (int j = 0; j < 4; j++) {
        usbMIDI.sendNoteOn(level1MajorChords[i][j], 100, 1);
      }
      digitalWrite(ledPinArray[i], HIGH);
      delay(Time * 12);
      digitalWrite(ledPinArray[i], LOW);
      for (int j = 0; j < 4; j++)
        usbMIDI.sendNoteOff(level1MajorChords[i][j], 0, 1);
    }
  } else {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}

void level1MinorChordMode() {
  if (on == true) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 4; j++) {
        usbMIDI.sendNoteOn(level1MinorChords[i][j], 100, 1);
      }

      digitalWrite(ledPinArray[i], HIGH);
      delay(Time * 12);
      digitalWrite(ledPinArray[i], LOW);
      for (int j = 0; j < 4; j++)
        usbMIDI.sendNoteOff(level1MinorChords[i][j], 0, 1);
    }
  } else {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}





void level2MajorArpeggioMode() {
  if (on == true) {

    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 4; j++) {
        delay(Time);
        usbMIDI.sendNoteOn(level2MajorChords[i][j], 100, 1);
        digitalWrite(ledPinArray[j], HIGH);
        delay(Time);
        usbMIDI.sendNoteOff(level2MajorChords[i][j], 0, 1);
        digitalWrite(ledPinArray[j], LOW);
      }
      for (int j = 2; j > 0; j--) {
        delay(Time);
        digitalWrite(ledPinArray[j], HIGH);
        usbMIDI.sendNoteOn(level2MajorChords[i][j], 100, 1);
        delay(Time);
        digitalWrite(ledPinArray[j], LOW);
        usbMIDI.sendNoteOff(level2MajorChords[i][j], 0, 1);

      }
    }
  } else  {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}
void level2MinorArpeggioMode() {
  if (on == true) {
    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 4; j++) {
        delay(Time);
        usbMIDI.sendNoteOn(level2MinorChords[i][j], 100, 1);
        digitalWrite(ledPinArray[j], HIGH);
        delay(Time);
        usbMIDI.sendNoteOff(level2MinorChords[i][j], 0, 1);
        digitalWrite(ledPinArray[j], LOW);
      }
      for (int j = 2; j > 0; j--) {
        delay(Time);
        digitalWrite(ledPinArray[j], HIGH);
        usbMIDI.sendNoteOn(level2MinorChords[i][j], 100, 1);
        delay(Time);
        digitalWrite(ledPinArray[j], LOW);
        usbMIDI.sendNoteOff(level2MinorChords[i][j], 0, 1);
      }
    }
  } else  {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}

void level2MajorChordMode() {
  if (on == true) {
    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 4; j++) {
        usbMIDI.sendNoteOn(level2MajorChords[i][j], 100, 1);
      }
      digitalWrite(ledPinArray[i], HIGH);
      delay(Time * 12);
      digitalWrite(ledPinArray[i], LOW);
      for (int j = 0; j < 4; j++)
        usbMIDI.sendNoteOff(level2MajorChords[i][j], 0, 1);
    }
  } else {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}


void level2MinorChordMode() {
  if (on == true) {
    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 4; j++) {
        usbMIDI.sendNoteOn(level2MinorChords[i][j], 100, 1);
      }

      digitalWrite(ledPinArray[i], HIGH);
      delay(Time * 12);
      digitalWrite(ledPinArray[i], LOW);
      for (int j = 0; j < 4; j++)
        usbMIDI.sendNoteOff(level2MinorChords[i][j], 0, 1);
    }
  } else {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}


void level3MajorArpeggioMode() {
  if (on == true) {

    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 4; j++) {
        delay(Time);
        usbMIDI.sendNoteOn(level3MajorChords[i][j], 100, 1);
        digitalWrite(ledPinArray[j], HIGH);
        delay(Time);
        usbMIDI.sendNoteOff(level3MajorChords[i][j], 0, 1);
        digitalWrite(ledPinArray[j], LOW);
      }
      for (int j = 2; j > 0; j--) {
        delay(Time);
        digitalWrite(ledPinArray[j], HIGH);
        usbMIDI.sendNoteOn(level3MajorChords[i][j], 100, 1);
        delay(Time);
        digitalWrite(ledPinArray[j], LOW);
        usbMIDI.sendNoteOff(level3MajorChords[i][j], 0, 1);

      }
    }
  } else  {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}
void level3MinorArpeggioMode() {
  if (on == true) {
    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 4; j++) {
        delay(Time);
        usbMIDI.sendNoteOn(level3MinorChords[i][j], 100, 1);
        digitalWrite(ledPinArray[j], HIGH);
        delay(Time);
        usbMIDI.sendNoteOff(level3MinorChords[i][j], 0, 1);
        digitalWrite(ledPinArray[j], LOW);
      }
      for (int j = 2; j > 0; j--) {
        delay(Time);
        digitalWrite(ledPinArray[j], HIGH);
        usbMIDI.sendNoteOn(level3MinorChords[i][j], 100, 1);
        delay(Time);
        digitalWrite(ledPinArray[j], LOW);
        usbMIDI.sendNoteOff(level3MinorChords[i][j], 0, 1);
      }
    }
  } else  {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}

void level3MajorChordMode() {
  if (on == true) {
    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 4; j++) {
        usbMIDI.sendNoteOn(level3MajorChords[i][j], 100, 1);
      }
      digitalWrite(ledPinArray[i], HIGH);
      delay(Time * 12);
      digitalWrite(ledPinArray[i], LOW);

      for (int j = 0; j < 4; j++)
        usbMIDI.sendNoteOff(level3MajorChords[i][j], 0, 1);
    }
  } else {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}


void level3MinorChordMode() {
  if (on == true) {
    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 4; j++) {
        usbMIDI.sendNoteOn(level3MinorChords[i][j], 100, 1);
      }
      digitalWrite(ledPinArray[i], HIGH);
      delay(Time * 12);
      digitalWrite(ledPinArray[i], LOW);

      for (int j = 0; j < 4; j++)
        usbMIDI.sendNoteOff(level3MinorChords[i][j], 0, 1);
    }
  } else {
    for (int i = 0; i < 12; i++) {
      digitalWrite(ledPinArray[i], LOW);
    }
  }
}

void checkOctaveNext() {
  if (millis() > lastOctaveStateChange + 50) {
    lastOctaveState = octaveState;
    octaveState = digitalRead(octaveNext);

    if (octaveState == HIGH && lastOctaveState == LOW) {
      lastOctaveStateChange = millis();

      ++counter;
    } else if (octaveState == LOW && lastOctaveState == HIGH) {
      lastOctaveStateChange = millis();

    }
  }
  if (counter > 0) digitalWrite(octaveNextLed, HIGH);
  else digitalWrite(octaveNextLed, LOW);
}

void checkOctavePrev() {
  if (millis() > lastOctaveStateChange1 + 50) {
    lastOctaveState1 = octaveState1;
    octaveState1 = digitalRead(octavePrev);

    if (octaveState1 == HIGH && lastOctaveState1 == LOW) {
      lastOctaveStateChange1 = millis();

      --counter;
    } else if (octaveState1 == LOW && lastOctaveState1 == HIGH) {
      lastOctaveStateChange1 = millis();

    }
  }
  if (counter < 0) digitalWrite(octavePrevLed, HIGH);
  else digitalWrite(octavePrevLed, LOW);
}

void displayKey(int k , int lev) {
  char displayFormattedNumber[4];
  char keyChar[2];
  if (k%12==0)strcpy(keyChar, "C");
  if (k%12==1)strcpy(keyChar, "0b");
  if (k%12==2)strcpy(keyChar, "0");
  if (k%12==3)strcpy(keyChar, "Eb");
  if (k%12==4)strcpy(keyChar, "E");
  if (k%12==5)strcpy(keyChar, "F");
  if (k%12==6)strcpy(keyChar, "F'");
  if (k%12==7)strcpy(keyChar, "G");
  if (k%12==8)strcpy(keyChar, "Ab");
  if (k%12==9)strcpy(keyChar, "A");
  if (k%12==10)strcpy(keyChar, "8b");
  if (k%12==11)strcpy(keyChar, "8");

  sprintf(displayFormattedNumber, "%2s" "%1d" "%1d", keyChar , 0, lev );
  Serial1.print(displayFormattedNumber );

}
void setBrightness(int number) {
  Serial1.write(0x7A);
  Serial.write(number);
}
void clearDisplay()
{
  Serial1.write(0x76);  // Clear display command
}
