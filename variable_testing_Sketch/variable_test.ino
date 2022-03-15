
//String must be in "" and single character must be in ''
char charTypeString[] = "char type";
String stringTypeString = "String type";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.print("This charTypeString: ");
  Serial.println(charTypeString);
  Serial.print("This is the size of charTypeString: ");
  Serial.println(sizeof(charTypeString));
  

  //char type requires you subtract one from the loop index to remove the null character
  for (int i = 0; i < sizeof(charTypeString)-1; i++){
    Serial.print("charTypeString ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(charTypeString[i]);

    //Try converting to int
    int charTypeStringToInt = charTypeString[i]; 
    Serial.print("charTypeString converted to int ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(charTypeStringToInt);

    //check if alphanumeric
    Serial.print("charTypeString ");
    Serial.print(i);
    Serial.print(" isAlphaNumeric: ");
    Serial.println(isAlphaNumeric(charTypeString[i]));

    //check if character is a space
    Serial.print("charTypeString ");
    Serial.print(i);
    Serial.print(" isSpace: ");
    Serial.println(isSpace(charTypeString[i]));

    //Try to capitalize 
    //To Capitalize you must declare a new char variable then subtract 32 from the original array
    //You cannot just subtract 32 from the array on the fly or the value is an integer instead of a character
    Serial.print("Subtracting 32 from char: ");
    char charTypeStringCap = charTypeString[i] - 32;
    Serial.println(charTypeStringCap);
    
    //Add a newline to make things more readable
    Serial.println("");
  }

//---------------------------------------------------------------------------------------------------------------

  Serial.println("-------------------------------------------------------------");
  Serial.print("This stringTypeString: ");
  Serial.println(stringTypeString);
  //sizeof() does not work on strings you must use the length function
  Serial.print("This is the length of the string using .length function: ");
  Serial.println(stringTypeString.length());
  
  //String type does not require the removal of null character so no need to subtract from the loop index
  for (int j = 0; j < stringTypeString.length(); j++){
    Serial.print("stringTypeString ");
    Serial.print(j);
    Serial.print(": ");
    Serial.println(stringTypeString[j]);

    //Try converting to int
    int stringTypeStringToInt = stringTypeString[j]; 
    Serial.print("stringTypeString converted to int ");
    Serial.print(j);
    Serial.print(": ");
    Serial.println(stringTypeStringToInt);

    //check if alphanumeric
    Serial.print("stringTypeString ");
    Serial.print(j);
    Serial.print(" isAlphaNumeric: ");
    Serial.println(isAlphaNumeric(stringTypeString[j]));

    //check if character is a space
    Serial.print("stringTypeString ");
    Serial.print(j);
    Serial.print(" isSpace: ");
    Serial.println(isSpace(stringTypeString[j]));

    //Try to capitalize 
    //Cannot Subtract in place
    //Cannot call .toUpperCase() in place
    //Must call .toUpperCase() for the string independently then reference array.
    //Should call .toUpperCase() outside of the loop so it doesn't apply repeatedly
    stringTypeString.toUpperCase();
    Serial.print("Capitalized String type: ");
    Serial.println(stringTypeString[j]);
    
    //Add a newline to make things more readable
    Serial.println("");
    
  }

}

void loop() {
  // put your main code here, to run repeatedly:


}
