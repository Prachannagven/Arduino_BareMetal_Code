////////////////////////////////////////////////
//Code to communicate with the DS3231 RTC Module
//Only uses the Wire.h library for I2C
//Highly reccomend watching this youtube video
//https://www.youtube.com/watch?v=Rd--iVQmu74
//Link to datasheet
//https://github.com/Prachannagven/Arduino_BareMetal_Code/blob/e37ee4cfbb1d9d709cf5a10fb551b5e8f8197a23/DS3231SN_Real_Time_Clock/DS3231_RTC_DataSheet.pdf
//I don't own this data sheet, but it's very nice
////////////////////////////////////////////////

#include <Wire.h>                                                   //Since we're using I2C communication, Wire.h is required to send and recieve the bits for the timing       

#define DS3231_ADDR 0x68                                            //This is the I2C address of the DS3231. It's always burned into the chip but in case you're using another model you can change this

//Defining the various registers where data can be read/written from. Highly suggest reading the data sheet as it very simply explains most of the data
  //Time storage registers
#define REG_SEC 0x00
#define REG_MIN 0x01
#define REG_HRS 0x02
#define REG_DOW 0x03
#define REG_DAT 0x04
#define REG_MNT 0x05
#define REG_YRS 0x06
  //Alarm 1 time registers
#define REG_ALRM1_SEC 0x07
#define REG_ALRM1_MIN 0x08
#define REG_ALRM1_HRS 0x09
#define REG_ALRM1_DAY 0x0A
  //Alarm 2 time registers
#define REG_ALRM2_MIN 0x0B
#define REG_ALRM2_HRS 0x0C
#define REG_ALRM2_DAY 0x0D

//When writing the initial time, store the timing values here.
byte START_SEC = 30;
byte START_MIN = 31;
byte START_HRS = 13;
byte START_DAY =  5;
byte START_DAT = 26;
byte START_MNT = 12;
byte START_YRS = 24;

//Selecting the mode of operation. If we use 12 hour operation, change the AM_PM bit accordingly and set the IS_12_HRS to 1. Setting to 0 makes it 24 hours
bool IS_12_HRS = 0;
bool AM_PM = 0;

//Calling the functions in the beginning of the script so that the compiler sets these up before moving into the setup and the loop functionalities
//The function actions will be explained later
byte DEC_TO_BCD(byte DECIMAL);
byte BCD_TO_DEC(byte BCD);
byte READ_REG(byte DEVICE_ADDR, byte REG_ADDR);                                                             
void WRITE_TIME(byte INP_SECOND, byte INP_MINUTE, byte INP_HOUR,
                bool AM, bool IS_12_HR, byte INP_DAY, byte INP_DATE,                                  
                byte INP_MONTH, byte INP_YEAR);



void setup() {
  Serial.begin(9600);                                               //To output in between commands and the final data, initialize serial monitor             
  Wire.begin();                                                     //Beginning the use of the Wire.h library

  WRITE_TIME(START_SEC, START_MIN, START_HRS,
            AM_PM, IS_12_HRS, START_DAY, START_DAT,                 
            START_MNT, START_YRS);                                  //Writing the initial time to the RTC

  delay(5000);                                                      //Waiting a few seconds before moving to the loop function
  

}

void loop() {
  READ_EVERYTHING_AND_PRINT();                                      //Constantly reading the time and printing it to the Serial Monitor
  delay(1000);                                                      //Waiting 1 second between readings
}

//Function to convert the decimal number into the appropriate format to load into the registers.
byte DEC_TO_BCD(byte DECIMAL){                                      
  int BCD_TENS = (DECIMAL/10);                                      //Getting the 10s place via simple integer division
  int BCD_ONES = DECIMAL - BCD_TENS*10;                             //Gettnig the 1s place via some basic math

  byte BCD =  BCD_ONES | (BCD_TENS << 4);                           //Combining the tens place and the ones place, into the requisit format of XXXXYYYY, X being tens and Y being ones

  return BCD;                                                       //Returning the value to wherever the function was called
}

//Function to convert back from the format loaded into a decimal number we'd understand
byte BCD_TO_DEC(byte BCD){
  return ((BCD >> 4) * 10) + (BCD & 0x0F);
}

//Function to read a particular register and return the byte that was stored in that register
byte READ_REG(byte DEVICE_ADDR, byte REG_ADDR){                     
  Wire.beginTransmission(DEVICE_ADDR);                              //Beginning to write to the specific address on the bus
  Wire.write(REG_ADDR);                                             //Now selecting the address we want to register
  Wire.endTransmission();                                           //Closing the writing line

  Wire.requestFrom(DEVICE_ADDR, 1);                                 //Now requestng the device to send back data from the register we called from
  while(Wire.available() == 0);                                     //Waiting for the data line to be pulled high before reading the data in case something happens
  
  return(Wire.read());                                              //Returning the byte read from the register. This works here because each register only contains one byte of data. In other sensors you may need to use a different set of commands.
}

//Function to write the starting time to the RTC module, we input the second, minute, hour, as well as the hour condiitions, day date month and year
void WRITE_TIME(byte INP_SECOND, byte INP_MINUTE, byte INP_HOUR,
                bool AM, bool IS_12_HR, byte INP_DAY, byte INP_DATE,
                byte INP_MONTH, byte INP_YEAR){
  //Seconds Writing
  Wire.beginTransmission(DS3231_ADDR);                              //Beginning transmission to the DS3231
  Wire.write(REG_SEC);                                              //Selecting the seconds register
  Wire.write(DEC_TO_BCD(INP_SECOND));                               //Converting the input seconds into the BCD format and writing to the register
  Wire.endTransmission();                                           //Ending the transmission
  Serial.println(DEC_TO_BCD(INP_SECOND), BIN);                      //Printing out the binary value of the seconds uploaded
  Serial.println("Uploaded seconds");                               //Confirmation method

  //Minutes Writing - Process is the same as the seconds so I won't elaborate again
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(REG_MIN);
  Wire.write(DEC_TO_BCD(INP_MINUTE));
  Wire.endTransmission();
  Serial.println(DEC_TO_BCD(INP_MINUTE), BIN);
  Serial.println("Uploaded minutes");

  //Hours Writing - This is slightly more complicated
  byte HRS_CMD = 0;                                                                         //We initialize a new variable to store the byte to transfer
  if(IS_12_HR) HRS_CMD = (IS_12_HR << 1 | AM);                                              //Begin with the top two bits of the command, based on the 12 HR and AM command
  else if(!IS_12_HR) HRS_CMD = ((IS_12_HR << 1) | (INP_HOUR/10 == 2) ? 1 : 0);              //If we're in 24 hour mode, do some bit operations to get the top 2
  HRS_CMD = (HRS_CMD << 1) | ((INP_HOUR/10 == 1) ? 1 : 0);                                  //Third bit which tells us whether the tens place is in use, this is in operation for both 10 and 24 hour mode
  HRS_CMD = (HRS_CMD << 4) | (0b00001111 & DEC_TO_BCD(INP_HOUR));                           //Final 4 bits for the ones place of the hour. And the BCD value with 0b00001111 to only take the bottom nibble
  Wire.beginTransmission(DS3231_ADDR);                                                      //Now follow the same upload procedure
  Wire.write(REG_HRS);
  Wire.write(HRS_CMD);
  Wire.endTransmission();
  Serial.println(HRS_CMD, BIN); 
  Serial.println("UPloaded Hours");

  //Day Writing - Same as the minutes and seconds writing, except we can just directly write the day without BCD conversion 
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(REG_DOW);
  Wire.write(INP_DAY);
  Wire.endTransmission();
  Serial.println(INP_DAY, BIN);
  Serial.println("Uploaded Day");

  //Date Writing - Same as minutes and seconds
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(REG_DAT);
  Wire.write(DEC_TO_BCD(INP_DATE));
  Wire.endTransmission();
  Serial.println(DEC_TO_BCD(INP_DATE), BIN);
  Serial.println("Uploaded Date");

  //Month Writing - Same as minutes and seocnds
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(REG_MNT);
  Wire.write(DEC_TO_BCD(INP_MONTH));
  Wire.endTransmission();
  Serial.println(DEC_TO_BCD(INP_MONTH), BIN);
  Serial.println("Uploaded Month");

  //Year Writing - Same as everything else
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(REG_YRS);
  Wire.write(DEC_TO_BCD(INP_YEAR));
  Wire.endTransmission();
  Serial.println(DEC_TO_BCD(INP_YEAR), BIN);
  Serial.println("Uploaded Year");

  Serial.println("Current date and time set");                      //Confirmation message to tell the user that the date and time have been set    
}

//Function to read the overall time from the RTC module and print it onto 
void READ_EVERYTHING_AND_PRINT(){
  //Using the READ_REG function to read the various registers and get the data stored on them
  byte READ_SEC = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_SEC));
  byte READ_MIN = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_MIN));
  byte READ_HRS = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_HRS) & 0x3F);  //Masking out the control bits so we have just data 
  byte READ_DOW = READ_REG(DS3231_ADDR, REG_DOW);
  byte READ_DAT = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_DAT));
  byte READ_MNT = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_MNT) & 0x1F);  //Masking out the control bits so we have just data
  byte READ_YRS = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_YRS));

  //Creating an array that stores the day of the week as strings so that we can later print it. note that this sequence has to correspond to the standard you're using
  const char* daysOfWeek[] = {
      "Sunday", "Monday", "Tuesday", "Wednesday", 
      "Thursday", "Friday", "Saturday"
  };

  // Print time in HH:MM:SS format
  Serial.print("Time: ");
  if (READ_HRS < 10) Serial.print('0');
  Serial.print(READ_HRS);
  Serial.print(':');
  if (READ_MIN < 10) Serial.print('0');
  Serial.print(READ_MIN);
  Serial.print(':');
  if (READ_SEC < 10) Serial.print('0');
  Serial.println(READ_SEC);

  // Print date in DD/MM/YY format
  Serial.print("Date: ");
  if (READ_DAT < 10) Serial.print('0');
  Serial.print(READ_DAT);
  Serial.print('/');
  if (READ_MNT < 10) Serial.print('0');
  Serial.print(READ_MNT);
  Serial.print('/');
  Serial.print(2000 + READ_YRS); // DS3231 returns years as offset from 2000

  // Print day of the week
  Serial.print(" (");
  if (READ_DOW >= 1 && READ_DOW <= 7) {
    Serial.print(daysOfWeek[READ_DOW - 1]); // Adjust 1-indexed value
  } else {
    Serial.print("Invalid Day");
  }
  Serial.println(')');
}


//Additional functionality so that you can read just a single data point and have it return to a value
byte READ_SECONDS(){
  byte READ_SEC = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_SEC));
  return READ_SEC;
}

byte READ_MINUTES(){
  byte READ_MIN = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_MIN));
  return READ_MIN;
}

byte READ_HOURS(){
  byte READ_HRS = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_HRS) & 0x3F);
  return READ_HRS;
}

byte READ_DAY(){
  byte READ_DOW = READ_REG(DS3231_ADDR, REG_DOW);
  return READ_DOW;
}

byte READ_DATE(){
  byte READ_DAT = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_DAT));
  return READ_DAT;
}

byte READ_MONTH(){
  byte READ_MNT = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_MNT) & 0x1F);
  return READ_MNT;
}

byte READ_YEAR(){
  byte READ_YRS = BCD_TO_DEC(READ_REG(DS3231_ADDR, REG_YRS));
  return READ_YRS;
}