#include <Wire.h>

#define OLED_ADDR 0x3C  // Replace with your display's I2C address

//Ensuring all the functions are compiled before the void setup begins as we've defined the functions post the main body of the code
void START_SCROLLING_VERTI_HORI(bool IS_LEFT, bool VERT_ONLY = 0, int SCROLL_GAP = 6, int PG_START = 1, int PG_END = 8, int VERT_OFFSET = 1);
void START_SCROLLING_HORI(bool IS_LEFT, int PG_START = 1, int SCROLL_GAP = 6, int PG_END = 8);
void CUSTOM_RECTANGLE(int WIDTH, int HEIGHT, int HEIGHT_OFFSET = 0, int WIDTH_OFFSET = 0);
void MIDDLE_SQUARE();
void ADDRESSAL_MODE(byte MODE);
void START_COL(byte LOW_NIB, byte HIGH_NIB);
void SEND_CMD(byte INP_CMD, byte DATA_1 = 0, byte DATA_2 = 0, byte DATA_3 = 0, byte DATA_4 = 0, byte DATA_5 = 0, byte DATA_6 = 0, byte DATA_7 = 0);
void SET_CONTRAST(byte CONTRAST);
void CLEAR_DISPLAY();
void INIT_DISPLAY();

void setup() {
  Serial.begin(9600);
  Wire.begin();

  INIT_DISPLAY();                                 //Initializing the display according to page 64 of the data sheet                                                                                           
  ADDRESSAL_MODE(0x02);                           //Setting addressal mode to page addressal
  START_COL(0x00, 0x10);                          //Setting the start column to be the first column in the OLED

  CLEAR_DISPLAY();                                //Clearing the GDDRAM of any data before beginning to write to the display
  uint8_t array[] ={};

  MIDDLE_SQUARE();
  START_SCROLLING_VERTI_HORI(1, 1);
  delay(5000);
  START_SCROLLING_HORI(0);
}

void loop() {
}

void START_SCROLLING_VERTI_HORI(bool IS_LEFT, bool VERT_ONLY = 0, int SCROLL_GAP = 6, int PG_START = 1, int PG_END = 8, int VERT_OFFSET = 1){
  if(VERT_ONLY){PG_START = 1; PG_END = 1;}
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);
  Wire.write(41+IS_LEFT);
  Wire.write(0x00);
  Wire.write(PG_START);
  Wire.write(SCROLL_GAP);
  Wire.write(PG_END);
  Wire.write(VERT_OFFSET);
  Wire.endTransmission();
  SEND_CMD(0x2F);
}
void START_SCROLLING_HORI(bool IS_LEFT, int PG_START = 1, int SCROLL_GAP = 6, int PG_END = 8){
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);
  Wire.write(38+IS_LEFT);
  Wire.write(0x00);
  Wire.write(PG_START);
  Wire.write(SCROLL_GAP);
  Wire.write(PG_END);
  Wire.write(0x00);
  Wire.write(0xFF);
  Wire.write(0x2F);
  Wire.endTransmission();
}


void CUSTOM_RECTANGLE(int WIDTH, int HEIGHT, int HEIGHT_OFFSET = 0, int WIDTH_OFFSET = 0){
  uint8_t FILLED_ARRAY[64][128] = {0};
  int MAX_WIDTH = WIDTH_OFFSET + WIDTH;
  int MAX_HEIGHT = HEIGHT_OFFSET + HEIGHT;

  for(int i = WIDTH_OFFSET ; i < WIDTH + WIDTH_OFFSET ; i++){
    for(int j = HEIGHT_OFFSET ; j < HEIGHT + HEIGHT_OFFSET; j++){
      FILLED_ARRAY[i][j] = 1;
    }
  }

  int MAX_PAGE = (MAX_HEIGHT/8);
  for(uint8_t PAGE = (HEIGHT_OFFSET/8) ; PAGE < (MAX_PAGE); PAGE++){
    uint8_t PAGE_ADJ = PAGE + 176;
    uint8_t UPPER_NIBBLE = WIDTH_OFFSET & 0b00001111;
    uint8_t LOWER_NIBBLE = (0b0001 << 4) | ((WIDTH_OFFSET & 11110000) >> 4);
    SEND_CMD(LOWER_NIBBLE);
    SEND_CMD(UPPER_NIBBLE);
    SEND_CMD(PAGE_ADJ);
    for(uint8_t COL = 0 ; COL < WIDTH ; COL ++){
      Wire.beginTransmission(OLED_ADDR);
      Wire.write(0x40);
      Wire.write(0b11111111);
      Wire.endTransmission();
    }
  }
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
      //delay(50);                                //Adding a small delay so that you can see the writing onto the display
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

//Function to clear the display
void CLEAR_DISPLAY(){
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
  SEND_CMD(0x2E);             //Stopping scrolling in case anything is happening
  SEND_CMD(0xAF);             //Turn Display on
}
