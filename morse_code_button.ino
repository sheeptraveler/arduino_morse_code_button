#include "SevSeg.h"
/***********************************************************************************************************************/
/* CONSTANTS */
/***********************************************************************************************************************/
SevSeg sevseg;
const int A = 7; 
const int B = 8;
const int C = 9;
const int D = 10;
const int E = 11;
const int F = 13; 
const int G = 12;
byte segmentPins[] = {A, B, C, D, E, F, G};

const int button_MC = 2;
const int button_EOL = 3;
const int button_EOW = 4;
const int button_EOM = 8;

bool EOL = false;
bool EOW = false;
bool EOM = false;

String morseCode = "";
String message = "";
int pressStartTime = 0;
char currentElement = '\0';


// Dit duration ms (dot)
const int DIT = 500; //
// Dah duration ms (dash)
const int DAH = 1000;
// Acceptable delay
const unsigned int DELAY = 250;

// Morse code table
typedef struct {
  char letter;
  unsigned int morseCode;
} letterCode;

const letterCode MORSE_CODE_TABLE[] = {
  {'a',  0b101},    
	{'b',  0b11000},
	{'c',  0b11010},
	{'d',  0b1100},
	{'e',  0b10},
	{'f',  0b10010},
	{'g',  0b1110},
	{'h',  0b10000},
	{'i',  0b100},
	{'j',  0b10111},
	{'k',  0b1101},
	{'l',  0b10100},
	{'m',  0b111},
	{'n',  0b110},
	{'o',  0b1111},
	{'p',  0b10110},
	{'q',  0b11101},
	{'r',  0b1010},
	{'s',  0b1000},
	{'t',  0b11},
	{'u',  0b1001},
	{'v',  0b10001},
	{'w',  0b1011},
	{'x',  0b11001},
	{'y',  0b11011},
	{'z',  0b11100}
};

// Defining the 7-segment display characters
int sevSegChars[35][8] {  
              // A B C D E F G
	          {'a',1,1,1,0,1,1,1},//A 
	          {'b',0,0,1,1,1,1,1},//b 
	          {'c',1,0,0,1,1,1,0},//C 
	          {'d',0,1,1,1,1,0,1},//d 
	          {'e',1,0,0,1,1,1,1},//E 
	          {'f',1,0,0,0,1,1,1},//F 
	          {'g',1,0,1,1,1,1,0},//G 
	          {'h',0,1,1,0,1,1,1},//H 
	          {'i',0,0,0,0,1,1,0},//I 
	          {'j',1,1,1,1,1,0,0},//J 
	          {'l',0,0,0,1,1,1,0},//L 
	          {'m',0,0,1,0,1,0,1},//m 
	          {'n',0,1,1,0,1,0,1},//n
	          {'o',0,0,1,1,1,0,1},//o 
	          {'p',1,1,0,0,1,1,1},//P 
	          {'q',1,1,1,0,0,1,1},//q 
	          {'r',0,0,0,0,1,0,1},//r 
	          {'s',1,0,1,1,0,1,1},//S
	          {'t',1,0,0,0,1,1,0},//T 
	          {'u',0,1,1,1,1,1,0},//U
	          {'v',0,1,0,1,0,1,1},//v
	          {'w',1,0,1,1,1,0,0},//w 
	          {'y',0,1,0,1,1,1,1},//y 
	          {'z',1,1,0,1,1,0,1},//z 
	          }; 
/***********************************************************************************************************************/
/* FUNCTIONS */
/***********************************************************************************************************************/
/* MORSE CODE FUNCTIONS */
/***********************************************************************************************************************/
/*
 * This function counts the time the button is pressed
*/
unsigned long int countTimeButtonPress(int buttonPin){
  unsigned long int startTime = millis();
  unsigned long int endTime = startTime;

  while(digitalRead(buttonPin) == HIGH){
    endTime = millis();
 }

  return (endTime - startTime);
}

/*
 * Return the dit or dah depending on the time the button has been pressed
 */
char dit_Or_Dah(unsigned long int time){
  char element = '\0';

  if (time >= DIT-DELAY && time <DIT+DELAY)
    element = '*';
  else if (time >= DAH-DELAY && time <DAH+DELAY)
    element = '_';

  return element;
}

/*
 * Convert a morse code to a binary mask
 */
unsigned int morseCodetoBin(String morseCode){
  unsigned int morseCodeBin = 1;
  for (int i = 0; i < morseCode.length(); i++){
    if (morseCode[i] == '*'){
      morseCodeBin = morseCodeBin << 1;
    }
    else if (morseCode[i] == '_'){
      morseCodeBin = (morseCodeBin << 1) + 1;
    }
  }
  return morseCodeBin;
}

/*
 * Searchs for a letter in the binary mask morse code table
 */
char findLetterFromBinMorseCode(unsigned int morseCodeBin){
  for (int i = 0; i < sizeof(MORSE_CODE_TABLE)/sizeof(letterCode); i++){
    if (MORSE_CODE_TABLE[i].morseCode == morseCodeBin){
      return MORSE_CODE_TABLE[i].letter;
    }
  }

  return '\0';
}

/*
 * Converts a morse code to an ASCII character
 */
char morseToASCII(String morseCode){
  unsigned int morseCodeBin = morseCodetoBin(morseCode);
  char letter = findLetterFromBinMorseCode(morseCodeBin);
  return letter;
}
/***********************************************************************************************************************/
/* 7 SEGMENT FUNCTIONS */
/***********************************************************************************************************************/
/*
 * Write a letter to the 7-segment display
 */
void writeToSevSeg(char letter){
  // Index -1 means the word was not found in the table
  int index = -1;
 
  // Search for the letter in the sevSegChars table
  for (int i = 0; i < sizeof(sevSegChars)/sizeof(sevSegChars[0]); i++){
    if (sevSegChars[i][0] == letter){
      index = i;
      break;
    }
  }

  // If the letter was in the table, write the input value to the corresponding segments
  if (index != -1)
    for (int i = 0; i < sizeof(segmentPins)/sizeof(byte); i++){
     // NOTE: ANODE common means that the pins get lit if the voltage is low hence the negation in front of sevsegChars
     digitalWrite(segmentPins[i], !sevSegChars[index][i+1]);
  }
  else{
    // If the letter is not found, turn off all segments and print an error message
    for (int i = 0; i < sizeof(segmentPins)/sizeof(byte); i++){
      digitalWrite(segmentPins[i], LOW);
    }
    Serial.println("Letter " + String(letter) + " not found in the table.");
  }
}
/***********************************************************************************************************************/
void setup(){
  Serial.begin(9600);
  pinMode(button_MC, INPUT);
  pinMode(button_EOL, INPUT);
  pinMode(button_EOW, INPUT);
  pinMode(button_EOM, INPUT);

  byte numDigits = 1;
  byte digitPins[] = {6};
  bool resistorOnSegments = false;
  byte hardwareConfig = COMMON_ANODE; // Model 5161BS uses common anode 
  bool updateWithDelays = false;
  bool leadingZeros = false;
  bool disableDecPoint = true;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
}

void loop(){
/*  Morse Code receiver Control */
  // Read a morse code letter
  if (digitalRead(button_MC) == HIGH && EOL == false){
    currentElement = dit_Or_Dah(countTimeButtonPress(button_MC)); // convert the current button press to an elment
    if (currentElement != '\0'){ // only add the element to the letter if it is a valid one
      morseCode += currentElement;
      Serial.print("current buffer: ");
      Serial.println(morseCode); // only print when the morse code is updated
    }
  }

/*  EOL Control */
  // The EOL button is pressed, show the letter or the error message, then reset the morse code 
  if (digitalRead(button_EOL) == HIGH && EOL == false){
    EOL = true;
    if (morseCode.length() > 0){
      currentElement = morseToASCII(morseCode);
      // If the morses code is valid, add its letter to the message
      if (currentElement != '\0'){
        message += currentElement;
        Serial.println("morse code [" + morseCode + "]: " + currentElement);
        // Show the letter on the 7-segment display
        writeToSevSeg(currentElement);
      }
      else{
        Serial.println("The code: " + morseCode + " has no match.");
      }
    }
  }
  // The button is released
  else if (digitalRead(button_EOL) == LOW && EOL == true){
    EOL = false; // reset EOL flag to accept new letters
    morseCode = ""; // reset the morse code
  }

/*  EOW Control */
  if (digitalRead(button_EOW) == HIGH && EOW == false){
    EOW = true;
    message += ' '; // add a space to the word
    Serial.println(message);
  }
  else if (digitalRead(button_EOW) == LOW && EOW == true){
    EOW = false; // reset EOW flag
  }
}
