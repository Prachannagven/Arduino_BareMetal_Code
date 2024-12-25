#define R1 2
#define R2 3
#define R3 4
#define R4 5
#define C1 6
#define C2 7
#define C3 8
#define C4 9

const int buttonVals[4][4] = {                                //An array of all the buttons and their positions
  {1, 2, 3, 10},                                              //10 = A
  {4, 5, 6, 11},                                              //11 = B
  {7, 8, 9, 12},                                              //12 = C
  {13, 0, 14, 15}                                             //13 = *, 14 = #, 15 = D
};

bool buttonsPressedState[4][4];                                    //The array in order to store the buttons being pressed at that moment

int buttonsPressed[4][4];

void setup() {
  Serial.begin(9600);                                         //Starting serial monitor for troubleshooting and printing values

  for(int i = 2; i <= 5; i++){                                //Setting all the row control pins to outputs
    pinMode(i, OUTPUT);
  }

  for(int i = 6; i <= 9; i++){                                //Setting all the column checking pins to inputs
    pinMode(i, INPUT);
  }

  digitalWrite(R1, HIGH);
  int pressed[4] = {0, 0, 0, 0};
  for(int i = 6; i<=9; i++){
    pressed[i-6] = digitalRead(i);
    Serial.println(pressed[i-6]);
  }
}

void loop() {

  /*
  for(int i=2; i<=5; i++){                                //Checking through the multiplexed keypad by setting each row high
    for(int j=2; j<=5; i++){                              //Initially setting all the rows low at the beginning of each cycle
      digitalWrite(j, LOW);
    }

    digitalWrite(i, HIGH);                                     //Writing the particular row to high
    delay(50);
    for(int j = 6; j <= 9; i++){                              //Checking each column to see if it is high or low (if high then button pressed)
      if(digitalRead(j) == HIGH){                             //If high then button is pressed
        buttonsPressedState[i-2][j-6] = 1;                    //If pressed, we set that particular point in the array
      }
      else{
        buttonsPressedState[i-2][j-6] = 0;                    //If not pressed then set to 0
      }
    }
  }

  for(int i = 0; i <= 3; i++){
    for(int j = 0; j <= 3; i++){
      //buttonsPressed[i][j] = buttonVals[i][j] * buttonsPressedState[i][j];
      Serial.print(buttonsPressedState[i][j]);
    }
    Serial.println("");
  }

  delay(500);

  */
}



