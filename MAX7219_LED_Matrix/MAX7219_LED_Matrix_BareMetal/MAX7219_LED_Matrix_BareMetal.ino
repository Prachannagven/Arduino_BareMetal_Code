//Connect all pins to PWM able pins on the arduino
#define CLK 3
#define CS 5
#define DIN 6


//Defining the various registers within the MAX7219 chip
#define REG_NO_OP 0x00
#define REG_DIGIT_0 0x01
#define REG_DIGIT_1 0x02
#define REG_DIGIT_2 0x03
#define REG_DIGIT_3 0x04
#define REG_DIGIT_4 0x05
#define REG_DIGIT_5 0x06
#define REG_DIGIT_6 0x07
#define REG_DIGIT_7 0x08
#define REG_DECODE_MODE 0x09
#define REG_INTENSITY 0x0A
#define REG_SCAN_LIMIT 0x0B
#define REG_SHUTDOWN 0x0C
#define REG_DISP_TEST 0x0F


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                                                                       //Beginning serial monitor for troubleshooting

  //All the pins communicating with the MAX7219 are outputs
  pinMode(CLK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(DIN, OUTPUT);

  digitalWrite(CS, HIGH);                                                                   //CS being driven low means that some communication has started, setting it high at the start to ensure no random data gets written

  normalOperationMAX();                                                                     //Turning the display on in the event that it was turned off before
  clearMAX();                                                                               //Clearing the display in case any data was saved priorly (turn this off if you want to save some inforamtion)
  decodeModeMAX(0x00);                                                                      //Since we're working with an 8x8 LED matrix here, we don't need a BCD converter at all. We tell the BCD register to only take in inputs for the 8x8 register
  scanLimitMAX(7);                                                                          //Telling the chip to scan all 7 registers and display all of them, otherwise the code will not be displayed properly

  MAX_A();

}

void loop() {
}

//Function to shut down the LED Matrix. The prior state is saved before all LEDs are turned off
void shutdownMAX(){
  writeToMAX(REG_SHUTDOWN, 0);
}

//Function to restart the LED Matrix. The prior state to being shutdown is shown again
void normalOperationMAX(){
  writeToMAX(REG_SHUTDOWN, 1);
}

//Function to tell the MAX7219 chip which mode it's working in. In our case we'll usually keep it in the 8x8 type but if you're doing something retarded go for it
void decodeModeMAX(volatile byte COMMAND){                                                      //COMMAND is the command byte (Ref data sheet) and can be 0x00 = NO DECODE ; 0x01 = DECODE FOR DIGIT 0 ONLY ; 0x0F = DECODE FOR ONLY DIGITS 3-0 ; 0xFF = DECODE FOR ALL DIGITS
  writeToMAX(REG_DECODE_MODE, COMMAND);
}

//
void changeBrightnessMAX(volatile byte BRIGHTNESS){                                             //BRIGHTNESS is a four bit binary number. The brightness is divided into 16 bits. Don't be an idiot
  writeToMAX(REG_INTENSITY, BRIGHTNESS);
}

void displayTestMAX(volatile bool STATE){
  writeToMAX(REG_DISP_TEST, STATE);
}

void scanLimitMAX(volatile int SCAN_LIMIT){
  writeToMAX(REG_SCAN_LIMIT, SCAN_LIMIT);
}

void clearMAX(){
  for(int i = 1 ; i<=8 ; i++){
    writeToMAX(i, 0x00);
  }
}

//Function to write to the MAX7219 chip using the protocol outlined in the datasheet, taking the register to write to and the data
void writeToMAX(volatile byte REGISTER, volatile byte DATA){                    
  digitalWrite(CS, LOW);                                                                        //Moving the CS low to begin shifting bits into the 16 bit shift register on the MAX7219
  shiftOut(DIN, CLK, MSBFIRST, REGISTER);                                                       //Writing the register address onto the data line in sync with the clock (bit by bit) 
  shiftOut(DIN, CLK, MSBFIRST, DATA);                                                           //Writing the data to store in that address in sync with the clock (bit by bit)
  digitalWrite(CS, HIGH);                                                                       //Ending the writing protocol by pulling the CS pin high and subsequently pushing all the data on the shift register to the actual registers on the MAX7219
}

/*---------------------------Now for some fun patterns---------------------------*/
void heartBase(){
  writeToMAX(REG_DIGIT_0, 0b00011000);
  writeToMAX(REG_DIGIT_1, 0b00100100);
  writeToMAX(REG_DIGIT_2, 0b01000010);
  writeToMAX(REG_DIGIT_3, 0b10000001);
  writeToMAX(REG_DIGIT_4, 0b10000001);
  writeToMAX(REG_DIGIT_5, 0b10011001);
  writeToMAX(REG_DIGIT_6, 0b01100110);
  writeToMAX(REG_DIGIT_7, 0b00000000);
}

void heartTop(){
  writeToMAX(REG_DIGIT_1, 0b00011000);
  writeToMAX(REG_DIGIT_2, 0b00100100);
  writeToMAX(REG_DIGIT_3, 0b01000010);
  writeToMAX(REG_DIGIT_4, 0b10000001);
  writeToMAX(REG_DIGIT_5, 0b10000001);
  writeToMAX(REG_DIGIT_6, 0b10011001);
  writeToMAX(REG_DIGIT_7, 0b01100110);
  writeToMAX(REG_DIGIT_0, 0b00000000);
}

void swirlySpiralRight(int speed){
  for(int j = 0; j <= 16; j++){
    for(int i = 1; i <= 8; i++){
      writeToMAX(i, (0b1 << (i-8+j)));
    }
    delay(speed);
  }
}

void swirlySpiralLeft(int speed){
  for(int j = 16; j >= 0; j--){
    for(int i = 8; i >= 1; i--){
      writeToMAX(i, (0b1 << (i-8+j)));
    }
    delay(speed);
  }
}


/*---------------------------Defining Letters-------------------------------*/
void MAX_A(){
  writeToMAX(REG_DIGIT_7, 0b00011000);
  writeToMAX(REG_DIGIT_6, 0b00100100);
  writeToMAX(REG_DIGIT_5, 0b01000010);
  writeToMAX(REG_DIGIT_4, 0b01111110);
  writeToMAX(REG_DIGIT_3, 0b01000010);
  writeToMAX(REG_DIGIT_2, 0b01000010);
  writeToMAX(REG_DIGIT_1, 0b01000010);
  writeToMAX(REG_DIGIT_0, 0b01000010);
}

void MAX_B(){
  
}
