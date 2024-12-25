#define DATA 13

byte dataDHT[5];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                                 //Starting Serial Monitor just in case I'll need it
  pinMode(DATA, OUTPUT);                              //Setting the DATA Pin to be an output initially, will have to be changed later

  beginSignal();                                      //Sends te start signal to the DHT

  pinMode(DATA, INPUT);                               //Now that start signal has been sent, waiting on return signal

  for(int i = 0; i < 5; i++){                         //Reading the 40 bit data signal
    dataDHT[i] = readData();                          //Assigning each value in the dataDHT to the respective byte from the sensor
    Serial.println(dataDHT[i], BIN);                  //Printing out the value on the serial monitor to see
  }

  byte sum = checkSum(dataDHT);                       //Getting the checkSum of the data
  Serial.println(sum, BIN);

  if((sum&0b000011111111) == dataDHT[4]){             //Checking only the last 8 bits of the checkSum against the 5th byte
    Serial.println("Success");                        //Showing success if scucessful
  }
  else{
    Serial.println("Failure");                        //Showing failure if failed
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}


void beginSignal(){
  pinMode(DATA, OUTPUT);                              //Setting the pinmode to output in order to write the start signal onto the line
  digitalWrite(DATA, LOW);                            //Writing the low signal to the Data Pin
  delay(20);                                          //Sending the low signal to the DHT for 20 milliseconds for detection
  digitalWrite(DATA, HIGH);                           //Writing the high signal to the data pin to imply end of start signal from the MCU side
  delayMicroseconds(30);                              //Waiting between 20 and 40 microseconds as high for the DHT response
  pinMode(DATA, INPUT);                               //Now that the initiate signal has been sent to start, we're setting the data pin to recieve from the DHT11
}

//Function to read a single byte of data from the data line 
byte readData(){
  byte dataByte = 0;                                  //Initializing a byte that will eventually store the information

  for(int i = 1; i < 8; i++){                         //8 bits make a byte, so we loop 8 times to read all the bits into the byte
    while(digitalRead(DATA) == LOW);                  //Each bit starts with a 50 microsecond low signal. Here, we wait until the signal gets pulled high
    delayMicroseconds(30);                            //A 28 microsecond signal menas 0, and a 70 microsecond means 1. After waiting 35 microsdconds, we check the value again
    if(digitalRead(DATA) == HIGH){                    //Checking if the data line is still high after 35 microseconds
      dataByte = dataByte | (1 << (7-i));             //If the data line is high, we write a 1 to the corresponding position of the byte
    }
    while(digitalRead(DATA) == HIGH);                 //We wait to go into the next loop until the data line is pulled low again
  }

  return dataByte;                                    //Returns the byte

}


//Function used to check validity of data
byte checkSum(byte data[5]){                          //Function takes in an array of five elements to check
  byte sum = 0;                                       //Initializing a byte to check the sum
  for(int i = 0; i < 4; i++){                         //Adding all the elements together into the sum variable
    sum = dataDHT[i] + sum;
  }
  return sum;                                         //Returning the sum of the first four bits
}