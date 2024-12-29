#include <Wire.h>

#define OLED_ADDR 0x3C  // Replace with your display's I2C address

void SEND_CMD(byte INP_CMD, byte DATA_1 = 0, byte DATA_2 = 0, byte DATA_3 = 0, byte DATA_4 = 0, byte DATA_5 = 0, byte DATA_6 = 0, byte DATA_7 = 0);
void SET_CONTRAST(byte CONTRAST);
void SEND_DATA(uint8_t DATA);

void setup() {
  
  byte DISP_ARRAY [8][128] = {0};
  Serial.begin(9600);
  Wire.begin();

  for(int page = 0; page < 8; page ++){
    for(int col = 0; col < 128; col = col ++){
      DISP_ARRAY[page][col] = 0xFF;
    }
  }

  INIT_DISPLAY();                                 //Initializing the display according to page 64 of the data sheet                                                                                           
  ADDRESSAL_MODE(0x00);                           //Setting addressal mode to page addressal
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);
  Wire.write(0x21);
  Wire.write(0x00);
  Wire.write(0x7F);
  Wire.endTransmission();
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);
  Wire.write(0x22);
  Wire.write(0);
  Wire.write(7);
  Wire.endTransmission();
  CLEAR_DISPLAY();

  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);
  Wire.write(0xA5);
  Wire.endTransmission();
  delay(1000);
  SEND_CMD(0xA4);
  delay(1000);

  for(int i = 0 ; i < 8 ; i = i+1){
    for(int j = 0 ; j < 128; j = j+1){
      SEND_DATA(DISP_ARRAY[i][j]);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

void SEND_DATA(uint8_t DATA){
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x40);
  Wire.write(DATA);
  Wire.endTransmission();
}

void CLEAR_DISPLAY() {
  for (int page = 0; page < 8; page++) {  // Iterate through all 8 pages
    for (int col = 0; col < 128; col++) {  // Iterate through all columns in each page
      SEND_DATA(0x00);  // Write 0xFF to turn on all 8 vertical pixels in the column
    }
  }
}

void SEND_CMD(byte INP_CMD, byte DATA_1 = 0, byte DATA_2 = 0, byte DATA_3 = 0, byte DATA_4 = 0, byte DATA_5 = 0, byte DATA_6 = 0, byte DATA_7 = 0){
  Wire.beginTransmission(OLED_ADDR);                
  Wire.write(0x00);                               //Setting the writing bits of D/C# and R/W# to 0
  Wire.write(INP_CMD);                            //Writing the command to the I2C line

  //If there's a data message being sent through, then it'll be written, otherwise blank
  if(DATA_1) Wire.write(DATA_1);
  if(DATA_2) Wire.write(DATA_2);
  if(DATA_3) Wire.write(DATA_3);
  if(DATA_4) Wire.write(DATA_4);
  if(DATA_5) Wire.write(DATA_5);
  if(DATA_6) Wire.write(DATA_6);
  if(DATA_7) Wire.write(DATA_7);
  Wire.endTransmission();
}

//Function to change the addressal mode. 0x00 is Horizontal, 0x01 is Vertical, 0x02 is Page Addressing. 0x03 is an invalid command
void ADDRESSAL_MODE(byte MODE){
  if(MODE < 3){
    Wire.beginTransmission(OLED_ADDR);              //Ensuring that the MODE bit is within the valid criteria    
    Wire.write(0x00);
    Wire.write(0x20);
    Wire.write(0x00);
    Wire.endTransmission(); 
  }    
  else{
    Serial.println("Not valid addressal");        //Returning an error message if MODE is outside the valid criteria
  }
}

void INIT_DISPLAY(){
  SEND_CMD(0xA8, 0x3F);       //Setting multipler ratio to 64 - Can be anythnig from 0x0F (16 MUX) to 0x3F(64 MUX)
  SEND_CMD(0xD3, 0x00);       //Seting vertical offset to 0 - The data can be from 0x00 to 0x64 with 0x64 being a full shift
  SEND_CMD(0x7F);             //Settng the display start line to be at the top - can be from 0x40(bottom line) to 0x7F (top line)
  SEND_CMD(0xA1);             //Column 0 (left most) is segment 0 - set to 0xA1 for right most segment to be 0
  SEND_CMD(0xC8);             //Seting COM scannnig direction from top to bottom - set to 0xC8 for bottom to top
  SEND_CMD(0xDA, 0x12);       //Setting the COM pins config to be alternating, with no left-right inversion. Check command table for more details.
  SET_CONTRAST(0x7F);         //Setting contrast to 128, the default value. Can vary between 0 and 256        
  SEND_CMD(0xA4);             //Display to read from GDDRAM and not just have the entire screen on. Set to 0xA5 to make the screen fully lit
  SEND_CMD(0xA6);             //Display mode is normal and not inverted. Send 0xA7 to make it invered
  SEND_CMD(0xD5, 0x80);       //Seting the divide ratio D and the oscillator frequency F_{OSC} o 1 and 64. Check the command table for more details
  SEND_CMD(0x8D, 0x14);       //Enabling charge pump  
  SEND_CMD(0x2E);             //Stopping scrolling in case anything is happening
  SEND_CMD(0xAF);             //Turn Display on
}

void SET_CONTRAST(byte CONTRAST){
  SEND_CMD(0x81, CONTRAST);                       //Contrast can be from 0x00 to 0x7F i.e. 0 to 256
}
