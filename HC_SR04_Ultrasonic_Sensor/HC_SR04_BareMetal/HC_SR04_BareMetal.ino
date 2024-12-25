#define TRIG 6                                                  //Trigger Pin on the HC SR04 should be connected to pin 6 (or any PWM pin)
#define ECHO 5                                                  //Echo Pin on the HC SR04 should be connected to pin 5 on the arduino

void setup() {
  Serial.begin(9600);                                           //Initializing Serial Monitor for any troubleshooting

  pinMode(TRIG, OUTPUT);                                        //Trigger pin on the HC SR04 needs an input, so we set the arduino to ouptut a signal onto the line
  pinMode(ECHO, INPUT);                                         //Echo pin returns to us the distance data on it, so we have the arduino read that data

  digitalWrite(TRIG, LOW);                                      //Initially set the trigger pin to low initially

  beginRanging();                                               //Call the start function to tell the HC SR04 to send out its ultrasonic pulse to measure
  Serial.println("Intialization Pulse Sent");

  int PW = returnPulseWidth();                                  //calling the returnPulseWidth function to find the pulse width
  float distCM = (PW)/58;                                       //Finding the distance in centimeters using the information in the data sheet
  
  //Printing out the pulse width and the distance in centimeters on the serial monitor
  Serial.print("Pulse Width Is:");                           
  Serial.println(PW);
  Serial.print("Distance in centimeters is:  ");
  Serial.println(distCM);
  Serial.println("");

}

void loop() {
}

//Function to send the begin command to the HC SR04 from the arduino
void beginRanging(){
  digitalWrite(TRIG, HIGH);                                     //Setting the trigger line high to begin the 10 microsecond pulse
  delayMicroseconds(10);                                        //Waiting 10 microsecons before stopping
  digitalWrite(TRIG, LOW);                                      //Pulling the trigger line back low to signal the end of the "begin" command
}

//Function to find the pulse width of the returned pulse to later extract the data 
int returnPulseWidth(){
  while(digitalRead(ECHO) == LOW);                              //Waiting for the echo pin to go high
  int startTime = micros();                                     //Setting a variable that measures how much time has elapsed since the program has began running till the echo pin is pulled high
  while(digitalRead(ECHO) == HIGH);                             //Waiting for the echo pin to go low
  int endTime = micros();                                       //Setting  a variable that measures how much time has elapsed since the program has began running till the echo pin has been pulled low

  int pulseTime = endTime-startTime;                            //Finding the pulse width by taking the differnce between the start and the end of the pulse
  return pulseTime;                                             //Returning the pulse width back to the user
}