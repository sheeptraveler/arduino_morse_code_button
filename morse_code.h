#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <Arduino.h>
#include <SevSeg.h>
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
unsigned long int pressTime = 0;
char currentElement = '\0';


// Dit duration ms (dot)
const int DIT = 250; //
// Dah duration ms (dash)
const int DAH = 500;
// Acceptable delay
const unsigned int DELAY = 125;
// CLR buffer
const unsigned int CLR_BUFFER = 1000;

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
	          {'g',1,0,1,1,1,1,1},//G 
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
/* MORSE CODE FUNCTIONS */
/***********************************************************************************************************************/
/*
 * This function counts the time the button is pressed
*/
unsigned long int countTimeButtonPress(int buttonPin);
/*
 * Return the dit or dah depending on the time the button has been pressed
 */
char dit_Or_Dah(unsigned long int time);
/*
 * Convert a morse code to a binary mask
 */
unsigned int morseCodetoBin(String morseCode);
/*
 * Searchs for a letter in the binary mask morse code table
 */
char findLetterFromBinMorseCode(unsigned int morseCodeBin);
/*
 * Converts a morse code to an ASCII character
 */
char morseToASCII(String morseCode);
/***********************************************************************************************************************/
/* 7 SEGMENT FUNCTIONS */
/***********************************************************************************************************************/
/*
 * Write a letter to the 7-segment display
 */
void writeToSevSeg(char letter);

/*
 * Write a message to the 7-segment display
 */ 
 void writeMessageToSevSeg(String message);

#endif
