//I defined the same pins twice in order to ease reading and understanding through the code. They don't overlap as the define function simply stores the value
//In the event that the serial interface is not working, close the serial monitor, reupload the sketch, then open the serial monitor. 
#define IN1 3                                                                   //Corresponds to Input 1 on the ULN2003 driver (PINK)
#define IN2 5                                                                   //Corresponds to Input 2 on the ULN2003 driver (BLUE)
#define IN3 6                                                                   //Corresponds to Input 3 on the ULN2003 driver (YELLOW)
#define IN4 9                                                                   //Corresponds to Input 4 on the ULN2003 driver (ORANGE)
#define COIL_2 5                                                                //Corresponds to Coil 2 on my motor (PINK)
#define COIL_4 9                                                                //Corresponds to Coil 4 on my motor (BLUE)
#define COIL_3 6                                                                //Corresponds to Coil 3 on my motor (YELLOW)
#define COIL_1 3                                                                //Corresponds to Coil 1 on my motor (ORANGE)

#define CYCLE_ANGLE  0.703125                                                   //Each cycle covers this much of an angle
int STEP_COUNT = 0;                                                             //Initializes a variable we'll later use to count the number of steps
float ANGLE = 0;                                                                //Initializes a variable we'll later use to count the angle swept

float CURRENT_ANGLE = 0;                                                        //Stores the current angle of the servo 
float MOVE_TO = 0;                                                              //Provides a global variable for the angle to go to


#define DELAY 1                                                                 //Delay between microsteps, the shorter this is the faster the motor moves

void setup() {
  Serial.begin(9600);                                                           //Initializing serial monitor for user input

  //Setting all the pins to be outputs to control the servo
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  /* Don't do something like this, it'll break the board as too much current is being drawn
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  */

  //Setting all the inputs to low initially to prevent random movements
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("Initialized");                                                //Informs the user on the serial monitor that you can now enter commands
  
  CURRENT_ANGLE = 0;                                                            //Setting the existing angle to 0 for homing
}

void loop() {
  if(Serial.available() > 0){                                                   //Waits for input on the serial monitor
    String inputString = Serial.readString();                                   //Reads the input as a string in order to check for special commands
    inputString.trim();                                                         //Trims of the new line to make sure no extra commands are there
    if(inputString == "Set 0"){                                                 //Homing command is checked for
      CURRENT_ANGLE = 0;                                                        //Rehomes the stepper
      Serial.println("Current position is now home");                           //Outputs onto the serial monitor to state that the current position is now home
    }
    else{
      MOVE_TO = inputString.toFloat();                                          //Converts the string to a float
      Serial.print("Your angle is ");                                           //Tells the user that it's moved to that angle
      Serial.println(MOVE_TO);
      goToAngle(MOVE_TO);                                                       //Moves to the required angle
    }
  }
}

//Function to move to a specific angle
void goToAngle(volatile float INPUT_ANGLE){
  float ANGLE_TO_MOVE = INPUT_ANGLE - CURRENT_ANGLE;                            //Creates a variable to find the angle difference between the current angle and the angle to move to
  if(INPUT_ANGLE-CURRENT_ANGLE > 0){                                            //Checks if the angle to move should be clockwise
    for(ANGLE = 0; ANGLE <= ANGLE_TO_MOVE; ANGLE = ANGLE + CYCLE_ANGLE){        //Loops clockwise until the desired angle is achieved
      oneCycleClockwise();
    }
  }
  else if(ANGLE_TO_MOVE < 0){                                                   //Checks if the angle ot move is counterclockwise
    for(ANGLE = 0; ANGLE >= ANGLE_TO_MOVE; ANGLE = ANGLE - CYCLE_ANGLE){        //Loops counterclockwise until the desired angle is achieved
      oneCycleCounterClockwise();
    }
  }
  CURRENT_ANGLE = INPUT_ANGLE;                                                  //Sets the current angle to be the position we've now moved to
}

//Function to rotate the motor clockwise by a set amount
void rotateAngleClockwise(volatile float INPUT_ANGLE){
  for(ANGLE = 0; ANGLE <= INPUT_ANGLE; ANGLE = ANGLE + CYCLE_ANGLE){
    oneCycleClockwise();
  }
}

//Function to rotate the motor counterclockwise by a set amount
void rotateAngleCounterClockwise(volatile int INPUT_ANGLE){
  for(ANGLE = 0; ANGLE <= INPUT_ANGLE; ANGLE = ANGLE + CYCLE_ANGLE){
    oneCycleCounterClockwise();
  }
}

//Function to rotate the motor clockwise by 360 degrees
void singleRotClockwise(){
  for(ANGLE = 0; ANGLE <= 360; ANGLE = ANGLE + CYCLE_ANGLE){
    oneCycleClockwise();
  }
}

//Function to rotate the motor counterclockwise by 360 degrees
void singleRotCounterClockwise(){
  for(ANGLE = 0; ANGLE <= 360; ANGLE = ANGLE + CYCLE_ANGLE){
    oneCycleCounterClockwise();
  }
}

//Completes 8 steps for one cycle of the magnets turning the shaft clockwise by 0.703125 degrees
void oneCycleClockwise(){
  //To get the motor to turn, we need to connect each coil to ground in the order 1-12-2-23-3-34-4-41
  //The ULN2003 chip that is being used for control consists of an inverted output. If the input is high, the output is low.
  //There are a lot of unnecessary lines here that I've left for understanding for a beginner
  
  //Step 1
  digitalWrite(COIL_1, HIGH);
  digitalWrite(COIL_4, LOW);
  //Step 2
  delay(DELAY);
  digitalWrite(COIL_1, HIGH);
  digitalWrite(COIL_2, HIGH);
  //Step 3
  delay(DELAY);
  digitalWrite(COIL_1, LOW);
  digitalWrite(COIL_2, HIGH);
  //Step 4
  delay(DELAY);
  digitalWrite(COIL_2, HIGH);
  digitalWrite(COIL_3, HIGH);
  //Step 5
  delay(DELAY);
  digitalWrite(COIL_2, LOW);
  digitalWrite(COIL_3, HIGH);
  //Step 6
  delay(DELAY);
  digitalWrite(COIL_3, HIGH);
  digitalWrite(COIL_4, HIGH);
  //Step 7
  delay(DELAY);
  digitalWrite(COIL_3, LOW);
  digitalWrite(COIL_4, HIGH);
  //step 8
  delay(DELAY);
  digitalWrite(COIL_4, HIGH);
  digitalWrite(COIL_1, HIGH);
  
  delay(DELAY);                                                                 //Delay for looping purposes        
}

//Completes 8 steps for one cycle of the magnets turning the shaft counterclockwise by 1.40625 degrees
void oneCycleCounterClockwise(){
  //To get the motor to turn, we need to connect each coil to ground in the order 4-34-3-23-2-12-1-41
  //The ULN2003 chip that is being used for control consists of an inverted output. If the input is high, the output is low.
  //There are a lot of unnecessary lines here that I've left for understanding for a beginner

  //Step 1
  digitalWrite(COIL_4, HIGH);
  digitalWrite(COIL_1, LOW);
  //Step 2
  delay(DELAY);
  digitalWrite(COIL_3, HIGH);
  digitalWrite(COIL_4, HIGH);
  //Step 3
  delay(DELAY);
  digitalWrite(COIL_4, LOW);
  digitalWrite(COIL_3, HIGH);
  //Step 4
  delay(DELAY);
  digitalWrite(COIL_2, HIGH);
  digitalWrite(COIL_3, HIGH);
  //Step 5
  delay(DELAY);
  digitalWrite(COIL_3, LOW);
  digitalWrite(COIL_2, HIGH);
  //Step 6
  delay(DELAY);
  digitalWrite(COIL_1, HIGH);
  digitalWrite(COIL_2, HIGH);
  //Step 7
  delay(DELAY);
  digitalWrite(COIL_2, LOW);
  digitalWrite(COIL_1, HIGH);
  //Step 8
  delay(DELAY);
  digitalWrite(COIL_4, HIGH);
  digitalWrite(COIL_1, HIGH);
  
  delay(DELAY);                                                                 //Delay for looping purposes            
}
