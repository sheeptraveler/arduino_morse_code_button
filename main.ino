#include <SevSeg.h>
#include "morse_code.h"
/***********************************************************************************************************************/
void setup(){
  Serial.begin(9600);
  // Button setup
  pinMode(button_MC, INPUT);
  pinMode(button_EOL, INPUT);
  pinMode(button_EOW, INPUT);
  
  // SevSeg setup
  byte numDigits = 1;
  byte digitPins[] = {6};
  bool resistorOnSegments = false;
  byte hardwareConfig = COMMON_ANODE; // Model 5161BS uses common anode 
  bool updateWithDelays = false;
  bool leadingZeros = false;
  bool disableDecPoint = true;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);
}

void loop(){
/*  Morse Code receiver Control */
  // Read a morse code letter
  if (digitalRead(button_MC) == HIGH && EOL == false){
    pressTime = countTimeButtonPress(button_MC); // count the time the button is pressed
    // Check to see if the user is trying to clear the buffer
    if (pressTime >= CLR_BUFFER){
      morseCode = ""; // reset the morse code if the button is pressed for too long
      Serial.println("Buffer cleared.");
    }
    else{
      currentElement = dit_Or_Dah(pressTime); // convert the current button press to an elment
      if (currentElement != '\0'){ // only add the element to the letter if it is a valid one
        morseCode += currentElement;
        Serial.print("current buffer: ");
        Serial.println(morseCode); // only print when the morse code is updated
      }
    }
  }

/*  EOL Control */
  // The EOL button is pressed, show the letter or the error message, then reset the morse code 
  if (digitalRead(button_EOL) == HIGH && EOL == false){
    EOL = true; // Control of bouncing, this way everything below is executed only once
    // Check to see if there is something in the morse code buffer
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
    EOL = false; // reset EOL flag to accept new letters and to avoid bouncing
    morseCode = ""; // reset the morse code
  }

/*  EOW Control */
  if (digitalRead(button_EOW) == HIGH && EOW == false){
    pressTime = countTimeButtonPress(button_EOW); 
    // Check to see if the user is trying to clear the message buffer
    if (pressTime >= CLR_BUFFER){
      EOW = true; // Control of bouncing, so that everything below is executed only once
      message = "";
      Serial.println("Message buffer cleared.");
    }
    else if (EOW == false && digitalRead(button_EOW) == HIGH) {
      // Need to access element in the message buffer, but only if its not empty
      if (message.length() > 0){
        // Only add space if the last character is not a space
        if (message[message.length() - 1] != ' '){
          message += ' '; // add a space to the word
          Serial.println("message buffer: [ " + message + " ]");
        }
        // write the message to the 7-segment display
        writeMessageToSevSeg(message);
      }
    }
  }
  else if (digitalRead(button_EOW) == LOW && EOW == true){
    EOW = false; // reset EOW flag
  }
}

