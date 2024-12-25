#define RS 12
#define RW 11
#define EN 10
#define D4 5
#define D5 4
#define D6 3
#define D7 2 

int pins[11] = {D4, D5, D6, D7, EN, RW, RS};

uint16_t command = 0b0000000;


void setup() {
  Serial.begin(9600); //Initializing Serial Monitor

  for(int i = 0; i<=11; i++){ //Setting all pins in the array to be outputs
    pinMode(pins[i], OUTPUT);
  }

  const char* word = "Nose sock nose";
  uint16_t binaryBytes[50]; // Array to store binary bytes
  size_t resultSize = 0;

  processWord(word, binaryBytes, &resultSize); //Convert the word into an array of bytes stored in "binaryBytes"

  command = 0b00000100000;            //Setting the LCD to take commands using only 4 bits

  command = 0b00000000001;            //Clear command to wipe the screen
  writeDigital(command);
  delay(5);
  upload();
  delay(50);

  command = 0b00000111011;            //Command to initialize 2 line, 5x8 display, 8 line input
  writeDigitalFourBit(command);
  upload();

  command = 0b00000001111;            //Cursor on, blinking
  writeDigitalFourBit(command);
  upload();

  command = 0b00000000111;            //Setting entry mode to keep cursor in same spot and shift display by one character left.
  writeDigitalFourBit(command);
  upload();

  command = (0b0001 << 7) | (0x0f);   //Setting the initial position of the cursor. Each line has up to 16 spaces to write characters in. Addresses in first line go from 0x00 to 0x27 and second line are from 0x40 to 0x67.
  writeDigitalFourBit(command);
  upload();

  word = "Good morning ";             //Passing the words to display on the top line.
  processWord(word, binaryBytes, &resultSize);    //Processing the word into an array of binary
  for(int i = 0; i<resultSize; i++){
    command = binaryBytes[i];
    writeDigitalFourBit(command);
    upload();
    delay(50);
  }

  command = (0b0001 << 7) | (0x51);   //Setting the initial position of the cursor. Each line has up to 16 spaces to write characters in. Addresses in first line go from 0x00 to 0x27 and second line are from 0x40 to 0x67.
  writeDigitalFourBit(command);
  upload();

  command = 0b00000000110;            //Setting entry mode to shift cursor by one every character entry. Display does not move
  writeDigitalFourBit(command);
  upload();

  word = "Bonjour";                   //Passing the words to display on the bottom line
  processWord(word, binaryBytes, &resultSize);      //Processing the word to an array of binary
  for(int i = 0; i<resultSize; i++){
    command = binaryBytes[i];
    writeDigitalFourBit(command);
    upload();
    delay(50);
  }
}

void loop() {

}


//Function to process a word into an array of binary bytes.
void processWord(const char* word, uint16_t* result, size_t* resultSize) {
    if (word == NULL || word[0] == '\0') {
        // If the input is invalid or empty, set result size to 0
        *resultSize = 0;
        return;
    }

    size_t len = strlen(word);
    *resultSize = len; // Number of letters in the word

    for (size_t i = 0; i < len; i++) {
        char currentLetter = word[i];
        uint16_t base = 0b100; // Starting binary number (3 bits)

        // Concatenate the binary representation of the current letter (8 bits)
        result[i] = (base << 8) | currentLetter;
    }
}

//To write the data on the bus to the IR, we set enable pin high then low to write it.
void upload(){
  digitalWrite(EN, HIGH);
  delay(250);
  digitalWrite(EN, LOW);
  delay(250);
}


//Setting the bits of the binary number onto the bus
void writeDigitalFourBit(uint16_t binaryNumber) {
  int startPin = 0;
  binaryNumber >> 4;
  for (int i = 0+startPin; i <= 6+startPin; i++) {      //Writing the first four bits of the command to the line
    bool bitValue = (binaryNumber >> (i-startPin)) & 1; // Extract each bit
    digitalWrite(pins[i], bitValue); // Write bit to the corresponding pin
  }
  upload();
  binaryNumber = binaryNumber & 0b00000001111;
  for(int i=0+startPin; i<=6+startPin; i++){
    bool bitValue = (binaryNumber >> (i-startPin)) & 1; // Extract each bit
    digitalWrite(pins[i], bitValue); // Write bit to the corresponding pin 
  }
}


void writeDigital(uint16_t binaryNumber) {
  int startPin = 0;
  for (int i = 0+startPin; i <= 10+startPin; i++) {
    bool bitValue = (binaryNumber >> (i-startPin)) & 1; // Extract each bit
    digitalWrite(pins[i], bitValue); // Write bit to the corresponding pin
  }
}


//To print out the binary number during troubleshooting
void printBinary(uint16_t number) {
  for (int i = 16; i >= 0; i--) {
    // Shift the bits to the right and check if the bit at position i is 1 or 0
    if (number & (1 << i)) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
  }
}

