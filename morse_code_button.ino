// PSEUDO CODE
// Read button state

// the idea is to run the loop and only require time precision when the button is pressed, because only them the first letter is being read.
// permit the user to inform the EOW and them print the first letter to the monitor
// wait for the user to inform the EOM and them print the full message to the monitor 
// start a new message
//
// read a letter 
//  print int to the segment display
//  or inform that it could not be uderstood
//
//  wait for the next letter or the end of the word signal EOW
// 
//  repeat the above until the end of message signal EOM

/* CONSTANTS */

// Dit duration ms (dot)
const int DIT = 1000; //
// Dah duration ms (dash)
const int DAH = 3000;
// Space between dots and dashes
const int INTER_GAP = DIT;
// Space between letters
const int SHORT_GAP = DAH;
// Space between words
const int MEDIUM_GAP = DAH*2 + DIT; 
// Acceptable delay
const unsigned int DELAY = 500;
// Letter size
const unsigned int LETTER_MAX_SIZE = 5*sizeof(DAH)/1000;

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

/* FUNCTIONS */

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

char dit_Or_Dah(unsigned long int time){
  char element = '\0';

  if (time >= DIT-DELAY && time <DIT+DELAY)
    element = '*';
  else if (time >= DAH-DELAY && time <DAH+DELAY)
    element = '_';

  return element;
}

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

char findLetterFromBinMorseCode(unsigned int morseCodeBin){
  for (int i = 0; i < sizeof(MORSE_CODE_TABLE)/sizeof(letterCode); i++){
    if (MORSE_CODE_TABLE[i].morseCode == morseCodeBin){
      return MORSE_CODE_TABLE[i].letter;
    }
  }

  return '\0';
}

char morseToASCII(String morseCode){
  unsigned int morseCodeBin = morseCodetoBin(morseCode);
  char letter = findLetterFromBinMorseCode(morseCodeBin);
  return letter;
}
/***********************************************************************************************************************/

const int button_MC = 2;
const int button_EOL = 4;
const int button_EOW = 7;
const int button_EOM = 8;

bool EOL = false;
bool EOW = false;
bool EOM = false;

String morseCode = "";
String message = "";
int pressStartTime = 0;
char currentElement = '\0';

void setup(){
  Serial.begin(9600);
  pinMode(button_MC, INPUT);
  pinMode(button_EOL, INPUT);
  pinMode(button_EOW, INPUT);
  pinMode(button_EOM, INPUT);
}

void loop(){
/*  Morse Code receiver Control */
  // Read a morse code letter
  if (digitalRead(button_MC) == HIGH && EOL == false){
    currentElement = dit_Or_Dah(countTimeButtonPress(button_MC)); // convert the current button press to an elment
    if (currentElement != '\0'){ // only add the element to the letter if it is a valid one
      morseCode += currentElement;
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
        Serial.println(currentElement);
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
