#include "morse_code.h"
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
    // NOTE: Common anode means that the segments are off when the voltage is high
    for (int i = 0; i < sizeof(segmentPins)/sizeof(byte); i++){
      digitalWrite(segmentPins[i], HIGH);
    }
    Serial.println("Letter " + String(letter) + " not found in the table.");
  }
}

/*
 * Write a message to the 7-segment display
 */ 
 void writeMessageToSevSeg(String message){
  // Clear the display
  for (int i = 0; i < sizeof(segmentPins)/sizeof(byte); i++){
    digitalWrite(segmentPins[i], HIGH);
  }
  
  // Write the message to the display
  for (int i = 0; i < message.length(); i++){
    if (message[i] == ' '){
      // If there is a space, turn off all segments
      for (int j = 0; j < sizeof(segmentPins)/sizeof(byte); j++){
        digitalWrite(segmentPins[j], HIGH);
      }
    }
    else{
      writeToSevSeg(message[i]);
    }
    delay(500); // Wait in ms before showing the next letter
  }
 }
