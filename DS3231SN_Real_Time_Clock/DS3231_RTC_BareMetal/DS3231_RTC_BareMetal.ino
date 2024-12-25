#include <Wire.h>

#define DS3231_ADDR 0x68

#define REG_SEC 0x00
#define REG_MIN 0x01
#define REG_HRS 0x02
#define REG_DOW 0x03
#define REG_DAT 0x04
#define REG_MON 0x05
#define REG_YRS 0x06

#define REG_ALRM1_SEC 0x07
#define REG_ALRM1_MIN 0x08
#define REG_ALRM1_HRS 0x09
#define REG_ALRM1_DAY 0x0A

#define REG_ALRM2_MIN 0x0B
#define REG_ALRM2_HRS 0x0C
#define REG_ALRM2_DAY 0x0D

byte SEC = 30;
byte MIN = 30;
byte HRS = 23;

byte DEC_TO_BCD(byte DECIMAL);
byte READ_REG(byte DEVICE_ADDR, byte REG_ADDR);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(REG_SEC);
  Wire.write(DEC_TO_BCD(SEC));
  Wire.endTransmission();

  Serial.println(DEC_TO_BCD(SEC), BIN);
  Serial.println("UPloaded seconds");

  Wire.beginTransmission(DS3231_ADDR);
  Wire.requestFrom(REG_SEC, 1);
  Wire.read()
  


}

void loop() {
  // put your main code here, to run repeatedly:

}

byte DEC_TO_BCD(byte DECIMAL){
  int BCD_TENS = (DECIMAL/10);
  int BCD_ONES = DECIMAL - BCD_TENS*10;

  byte BCD =  BCD_ONES | (BCD_TENS << 4);

  return BCD;
}

byte READ_REG(byte DEVICE_ADDR, byte REG_ADDR){
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(REG_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(REG_ADDR, 1);
  while(Wire.available() == 0);
  
  return(Wire.read());
}
