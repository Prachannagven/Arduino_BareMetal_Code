#include <Wire.h>

#define OLED_ADDR 0x3C  // Replace with your display's I2C address

void SEND_CMD(byte INP_CMD, byte DATA_1 = 0, byte DATA_2 = 0, byte DATA_3 = 0, byte DATA_4 = 0, byte DATA_5 = 0, byte DATA_6 = 0, byte DATA_7 = 0);


void setup() {
  Serial.begin(9600);
  Wire.begin();

  INIT_DISPLAY();                                                                         
  SEND_CMD(0x20, 0x02);
  START_COL(0x00, 0x10);

  CLEAR_DISP();
  uint8_t array[] = {};
  
  MIDDLE_SQUARE();
}

void loop() {
}

void MIDDLE_SQUARE(){
  for(uint8_t page = 2 ; page < 6 ; page ++){
    byte PAGE_ADJUSTED = page + 176;              //Adjusting the integral page nuber into the correct binary form to load to the GDDRAM            
    //Upon page incrementation, the column pointer doesn't get reset, to fix that, we write this code
    SEND_CMD(0x00);                               //Lower bit of start column
    SEND_CMD(0x13);                               //Upper bit of start column
    SEND_CMD(PAGE_ADJUSTED);                      //Incrementing the page
    //Now we increment the column address
    for(uint8_t col = 32; col < 64; col ++){
        Wire.beginTransmission(OLED_ADDR);        
        Wire.write(0x40);                         //Sending the write prefix to inform the GDDRAM that the next command should be written
        Wire.write(0b11111111);                   //Setting all pixels in all COM lines to 1 in each column and page
        Wire.endTransmission();
        delay(50);                                //Adding a small delay so that you can see the writing onto the display
    }
  }
}

//Function to change the addressal mode. 0x00 is Horizontal, 0x01 is Vertical, 0x02 is Page Addressing. 0x03 is an invalid command
void ADDRESSAL_MODE(byte MODE){
  if(MODE < 3) SEND_CMD(0x20, MODE);              //Ensuring that the MODE bit is within the valid criteria         
  else{
    Serial.println("Not valid addressal");        //Returning an error message if MODE is outside the valid criteria
  }
}

//Function to set the starting column in page addressing mode. The starting column is set with the lower nibble and the upper nibble. We pass both and then the OLED knows where to start columing
void START_COL(byte LOW_NIB, byte HIGH_NIB){
  SEND_CMD(LOW_NIB);                              //Sending the lower nibble first
  SEND_CMD(HIGH_NIB);                             //Setting the upper niblle next
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

//Function to write the contrast value to the 0x81 byte
void SET_CONTRAST(byte CONTRAST){
  SEND_CMD(0x81, CONTRAST);                       //Contrast can be from 0x00 to 0x7F i.e. 0 to 256
}

void CLEAR_DISP(){
  //Setting start and end column in the page addressing mode. 0x00 is the 0th line and 0x7F is the 128th line.
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);
  Wire.write(0x21);
  Wire.write(0x00);
  Wire.write(0x7F);
  Wire.endTransmission();
  //Setting start and end page in page addressing mode. 0x00 is the 1st page and 0x07 is the 8th page.
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);
  Wire.write(0x22);
  Wire.write(0x00);
  Wire.write(0x07);
  Wire.endTransmission();

  //Writing the entire hostel blank by setting all the bits to 0x00.
  for(uint8_t page = 0 ; page < 8 ; page ++){
    SEND_CMD(page+176);       //In the page addressing mode, after the column reaches the end, the page does not increment. This line increments the page with the loop
    for(uint8_t col = 0; col < 128; col ++){
        Wire.beginTransmission(OLED_ADDR);
        Wire.write(0x40);     //Sending the write command as opposed to the read command to write to the GDDRAM 
        Wire.write(0x00);     //Sending the 0x00 to blank out the pixel in the GDDRAM
        Wire.endTransmission();
    }
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
  SEND_CMD(0xAF);             //Turn Display on
}
