//Program Control Variables
//Int to control the flow of the program
//0 = User Input
//1 = Processing
//2 = Encoding
//3 = Transmitting
//4 = Reset
int progStage = 0;

//Serial Text Collection Variables
//-----------------------------------------------------------------------------------------
//String variable to collect and store raw text input
//the variable should be 1 more than the limit to allow detection of overrun
//Should be reset by variable reset function
String txtCollection = "";
//bool to indicate that text collection has completed  
//Set to false once transmission is complete
bool txtCollectionComplete = false;

//Message Processing Variables
//-----------------------------------------------------------------------------------------
//Int to control the max characters allowed
int maxCharacters = 60;
//Bool to indicate that the message has been approved
//set to false once transmission complete
bool msgProcessingComplete = false;
//bool to stop processing if the message fails a condition
bool msgProcessingFailed = false;

//Encoding Processing Variables
//-----------------------------------------------------------------------------------------
//Bool to indicate that the message has been encoded
//Set to false once the transmission complete
bool morseEncodingComplete = false;
//String for accumulating the morse binary
String morseBinaryString = "";

//Transmission Processing Variable
//-----------------------------------------------------------------------------------------
//bool to indicate that the morse has finished transmitting
bool transmissionComplete = false;
//Int to store the tone frequency that the morse should be transmissed with
int msTone = 1000;
//int to store the time that each morse binary bit should play
//Based on the 5 character word "PARIS" - 20wpm requires a 60 ms dit(1)
int morseBinaryDelay = 40;

//++++++++++++++++
//Pin for sending out morse code tone
int codeOutPin = 6;

//-----------------------------------------------------------------------------------------
void setup() {
  //Start the Serial Connection
  Serial.begin(9600);

  //Set pin modes
  //Set pinMode for codeOutPin
  pinMode(codeOutPin, OUTPUT);
  digitalWrite(codeOutPin, LOW);

  //Reserve space for the txtCollection variable
  //txtCollection.reserve(2);
  
  //Introduce the program
  Serial.println(F("\n-----------------------------------------"));
  Serial.println(F("Morse Code Transcoder by Jonathan Ceis"));
  Serial.println(F("A project for Edmonds College - ENGR121"));
  Serial.println(F("-----------------------------------------"));
  Serial.println(F("Messages must be less than 60 characters long and contain a-z, A-Z, 0-9, and space characters\n"));
  Serial.println(F("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
  //Send the first prompt to enter text
  prompt4Message();
}

void loop() {

  //INPUT
  //Wait for text input
  //Once text is input proceed
  while (progStage == 0 && !txtCollectionComplete){
    //If data is in the serial buffer then read it
    //Otherwise wait
    if(Serial.available() > 0){
      readSerialData();
    } 
    else{
      //delay to prevent program from progressing
      delay(100);
    }
  }
  //if still in progStage 0 and text collection has been completed then update the program stage to 1
  if (progStage == 0 && txtCollectionComplete){
    progStage = 1;
  }

  //PROCESSING
  //Process the message once text collection has been completed
  if(progStage == 1 && !msgProcessingComplete){
    //call the function to process the message and set msgProcessingFailed to true if the text is not alphanumeric
    msgProcessingFailed = processMessage();
  }

  //if still in program stage 1 and processing has been completed check to see if msgProcessing failed
  //If message processing failed then go to program stage 4 to reset, otherwise proceed to stage 2 for encoding
  if (progStage == 1 && msgProcessingComplete){
    if(msgProcessingFailed){
      Serial.println(F("\n  --->!!! Transmission Aborted !!!!<---"));
      progStage = 4;
    }
    else {
      progStage = 2;
    }
  }

  //ENCODING
  //If processing is complete move on to encoding
  if(progStage == 2 && !morseEncodingComplete){
    Serial.print(F("  -Encoding ->          "));
    //Iterate through the txtCollection String and pass each character to the morseEncoder
    for(int i=0; i < txtCollection.length(); i++){
      Serial.print(F("#"));
      morseBinaryString += morseEncoder(txtCollection[i]);
    }

    Serial.print(F("\n  -Encoded Morse as Binary:  "));
    Serial.println(morseBinaryString);
 
    //Set the the more encoding to true so the program knows to move on
    morseEncodingComplete = true;
    
  }
  //If still in program stage 2 and morse encoding is complete then set program stage to 3
  if(progStage == 2 && morseEncodingComplete){
        progStage = 3;
  }

  //TRANSMISSION
  //Once Encoding is complete move to transmission
  if(progStage == 3 && !transmissionComplete){
    Serial.print(F("  -Transmitting:             "));
    //loop through the morse
    for(int i=0; i < morseBinaryString.length(); i++){
      morseTransmission(morseBinaryString[i]);
    }
    //Notify the user that transmission is complete
    Serial.println(F("\n  -Transmission Complete"));
        
    //Once the morse has been transmitted set transmissionComplete to true so the program knows to move on
    transmissionComplete = true;
  }

  //if still in stage 4 and transmission is complete then set the program stage to 4 to reset
  if(progStage == 3 && transmissionComplete){
        progStage = 4;
  }

  //RESET
  //Once the program has finished reset all the variables and prompt the user to enter text again
  if(progStage == 4){
    //Reset the variables
    resetProg();
    //Print a line of symbols to make it evident that the program is waiting for new input
    Serial.println(F("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
    //Prompt the user to enter additional text
    prompt4Message();
    //Change program state to 0 to accept serial input
    progStage = 0;
  }
}

/*-----------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------
 */

 //Function to prompt the user for input
 //Call function to reset variables
 void prompt4Message(){
  Serial.println(F("Please Enter Message to be Transcoded to Morse Code"));
 }

 //----------------------------------------------------------------------------------------
 //Function to read serial data 
 //Will be used for message input and confirming text
 //Raw input will be added to a collection
 //Check if character is \n
 //if yes, stop collecting
 //if no, then add raw input to collection and keep going
 
 void readSerialData(){
  //read the serial data character by character
  char rawInput = (char)Serial.read();

  //Check to see if the character is \n
  //If yes, then stop collecting, set txtControl to y/n and txtCollectionComplete to true
  //if no, then add to collection and keep going
  if (rawInput == '\n'){
    txtCollectionComplete = true;
  }
  else{
    txtCollection += rawInput;
  }
 }

 //----------------------------------------------------------------------------------------
 //Function to process message
 
 //Display the processed text
 bool processMessage(){
  //Create a bool to identify if input was invalid
  bool invalidInput = false;
  //Print what was input by the user
  Serial.print(F("  -Original User Input: "));
  Serial.println(txtCollection);

  //Remove any leading or trailing whitespace
  txtCollection.trim();

  //Ensure the text input is less than 60 characters
  if(txtCollection.length() > maxCharacters){
    Serial.println(F("  -WARNING - Message Maxiumum Exceeded! - Please enter less than 60 Characters"));
    //set invalid input to true if message is too long
    invalidInput = true;
  }

  //parse the text to make sure that it is alphanumeric and only contains spaces
  //If illegal character then prompt to re-enter input
  for(int i=0; i < txtCollection.length(); i++){
    //Check to see if any of the characters are not alphanumeric or a space
    if(!isAlphaNumeric(txtCollection[i]) && !isSpace(txtCollection[i])){
      Serial.print(F("  -WARNING - Message Contains Invalid Characters - Message should only contain a-z, A-Z, 0-9, and space characters. --> Given: "));
      Serial.println(txtCollection[i]);
      //set msgProcessingFailed to true this will be used in void loop to reset the program
      //msgProcessingFailed = true;
      invalidInput = true;
    }
  }
  if(!invalidInput){
    //If valid, change all characters to uppercase
    //Assign the approved message so the message isn't overwritten during confirmation
    txtCollection.toUpperCase();
  
    //print the processed message
    Serial.print(F("  -Processed Message:   "));
    Serial.println(txtCollection);
  }
  //Set the processing as complete so the program knows to move on
  msgProcessingComplete = true;
  //return the invalidInput value to void loop
  return invalidInput;
 }


 
 //----------------------------------------------------------------------------------------
 //Function to reset variables
 //Reset all text variables to ensure that old and new messages are not combined
 void resetProg(){
  //Set all stage statuses to false
  txtCollectionComplete = false;
  msgProcessingComplete = false;
  morseEncodingComplete = false;
  transmissionComplete = false;

  //Set Failure status to false
  msgProcessingFailed = false;

  //Reset text accumulators
  txtCollection.remove(0);
  morseBinaryString.remove(0);

  //Reset the progStage
  progStage = 0;
 }


 //----------------------------------------------------------------------------------------
 //Function to encode the message in morse
 //

 String morseEncoder(char x){


//----------------------------------------------------------------------------------------
 //Morse Code Variables
 //
 // Character - A Morse - Dit Dah Binary - 1000111000          
 String mcA = "1000111000";
 // Character - B Morse - Dah Dit Dit Dit Binary - 111000100010001000          
 String mcB = "111000100010001000";
 // Character - C Morse - Dah Dit Dah Dit Binary - 11100010001110001000          
 String mcC = "11100010001110001000";
 // Character - D Morse - Dah Dit Dit Binary - 11100010001000          
 String mcD = "11100010001000";
 // Character - E Morse - Dit Binary - 1000          
 String mcE = "1000";
 // Character - F Morse - Dit Dit Dah Dit Binary - 100010001110001000          
 String mcF = "100010001110001000";
 // Character - G Morse - Dah Dah Dit Binary - 1110001110001000          
 String mcG = "1110001110001000";
 // Character - H Morse - Dit Dit Dit Dit Binary - 1000100010001000          
 String mcH = "1000100010001000";
 // Character - I Morse - Dit Dit Binary - 10001000          
 String mcI = "10001000";
 // Character - J Morse - Dit Dah Dah Dah Binary - 1000111000111000111000          
 String mcJ = "1000111000111000111000";
 // Character - K Morse - Dah Dit Dah Binary - 1110001000111000          
 String mcK = "1110001000111000";
 // Character - L Morse - Dit Dah Dit Dit Binary - 100011100010001000          
 String mcL = "100011100010001000";
 // Character - M Morse - Dah Dah Binary - 111000111000          
 String mcM = "111000111000";
 // Character - N Morse - Dah Dit Binary - 1110001000          
 String mcN = "1110001000";
 // Character - O Morse - Dah Dah Dah Binary - 111000111000111000          
 String mcO = "111000111000111000";
 // Character - P Morse - Dit Dah Dah Dit Binary - 10001110001110001000          
 String mcP = "10001110001110001000";
 // Character -  Q Morse - Dah Dah Dit Dah Binary - 1110001110001000111000          
 String mcQ = "1110001110001000111000";
 // Character - R Morse - Dit Dah Dit Binary - 10001110001000          
 String mcR = "10001110001000";
 // Character - S Morse - Dit Dit Dit Binary - 100010001000          
 String mcS = "100010001000";
 // Character - T Morse - Dah Binary - 111000          
 String mcT = "111000";
 // Character - U Morse - Dit Dit Dah Binary - 10001000111000          
 String mcU = "10001000111000";
 // Character - V Morse - Dit Dit Dit Dah Binary - 100010001000111000          
 String mcV = "100010001000111000";
 // Character - W Morse - Dit Dah Dah Binary - 1000111000111000          
 String mcW = "1000111000111000";
 // Character - X Morse - Dah Dit Dit Dah Binary - 11100010001000111000          
 String mcX = "11100010001000111000";
 // Character - Y Morse - Dah Dit Dah Dah Binary - 1110001000111000111000          
 String mcY = "1110001000111000111000";
 // Character - Z Morse - Dah Dah Dit Dit Binary - 11100011100010001000          
 String mcZ = "11100011100010001000";
 // Character - 1 Morse - Dit Dah Dah Dah Dah Binary - 1000111000111000111000111000          
 String mc1 = "1000111000111000111000111000";
 // Character - 2 Morse - Dit Dit Dah Dah Dah Binary - 10001000111000111000111000          
 String mc2 = "10001000111000111000111000";
 // Character - 3 Morse - Dit Dit Dit Dah Dah Binary - 100010001000111000111000          
 String mc3 = "100010001000111000111000";
 // Character - 4 Morse - Dit Dit Dit Dit Dah Binary - 1000100010001000111000          
 String mc4 = "1000100010001000111000";
 // Character - 5 Morse - Dit Dit Dit Dit Dit Binary - 10001000100010001000          
 String mc5 = "10001000100010001000";
 // Character - 6 Morse - Dah Dit Dit Dit Dit Binary - 1110001000100010001000          
 String mc6 = "1110001000100010001000";
 // Character - 7 Morse - Dah Dah Dit Dit Dit Binary - 111000111000100010001000          
 String mc7 = "111000111000100010001000";
 // Character - 8 Morse - Dah Dah Dah Dit Dit Binary - 11100011100011100010001000          
 String mc8 = "11100011100011100010001000";
 // Character - 9 Morse - Dah Dah Dah Dah Dit Binary - 1110001110001110001110001000          
 String mc9 = "1110001110001110001110001000";
 // Character - 0 Morse - Dah Dah Dah Dah Dah Binary - 111000111000111000111000111000          
 String mc0 = "111000111000111000111000111000";
 // Character - Space Morse - M.Gap Binary - 0000          
 String mcSpace = "0000";

//Depending on which character is passed add the binary value to the morseBinaryString
  switch (x){
    case 'A':
      return mcA;
      break;
    case 'B':
      return mcB;
      break;
    case 'C':
      return mcC;
      break;
    case 'D':
      return mcD;
      break;
    case 'E':
      return mcE;
      break;
    case 'F':
      return mcF;
      break;
    case 'G':
      return mcG;
      break;
    case 'H':
      return mcH;
       break;
    case 'I':
      return mcI;
      break;
    case 'J':
      return mcJ;
      break;
    case 'K':
      return mcK;
      break;
    case 'L':
      return mcL;
      break;
    case 'M':
      return mcM;
      break;
    case 'N':
      return mcN;
      break;
    case 'O':
      return mcO;
      break;
    case 'P':
      return mcP;
      break;
    case 'Q':
      return mcQ;
      break;
    case 'R':
      return mcR;
      break;
    case 'S':
      return mcS;
      break;
    case 'T':
      return mcT;
      break;
    case 'U':
      return mcU;
      break;
    case 'V':
      return mcV;
      break;
    case 'W':
      return mcW;
      break;
    case 'X':
      return mcX;
      break;
    case 'Y':
      return mcY;
      break;
    case 'Z':
      return mcZ;
      break;
    case '0':
      return mc0;
      break;
    case '1':
      return mc1;
      break;
    case '2':
      return mc2;
      break;
    case '3':
      return mc3;
      break;
    case '4':
      return mc4;
      break;
    case '5':
      return mc5;
      break;
    case '6':
      return mc6;
      break;
    case '7':
      return mc7;
      break;
    case '8':
      return mc8;
      break;
    case '9':
      return mc9;
      break;
    case ' ':
      return mcSpace;
      break;
    default:
      Serial.println(F("Something has gone very wrong! Encoder received invalid character!"));
      break;
  }
  
//this is the end of the morse encoder
 }

 /*
  * Morse Transmission Function
  */
  
  void morseTransmission(char x){
    //print the character being processed
    Serial.print(x);
    //Determine if tone is supposed to be off or on
    if(x == '0'){
      noTone(codeOutPin);    
    }
    else if(x == '1'){
      tone(codeOutPin, msTone);
    }
    else{
      Serial.println(F("Something has gone horribly wrong! A non-binary character has reached the encoder"));
    }

    //Delay while the time that the morse binary character should be on or off
    delay(morseBinaryDelay);

  }

























  
