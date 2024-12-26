#define DIGITAL_PIN 3
#define ANALOG_PIN A4


void setup() {
  Serial.begin(9600);                                                 //Beginning Serial Monitor for printing values

  pinMode(DIGITAL_PIN, INPUT);                                        //Setting the digital output of the sound sensor to an input of the arduino
  pinMode(ANALOG_PIN, INPUT);                                         //Setting the analog output of the sound sensor to an input of the arduino
}

void loop() {
  bool DIGITAL_MODE = digitalRead(DIGITAL_PIN);                       //Reading the digital output of the sound sensor, set the trigger level using the potentiometer
  int ANALOG_MODE = analogRead(ANALOG_PIN);                           //Reading the analog ouput of the sound sensor

  //Printing in some sane format
  Serial.print("Digital Trigger = ");
  Serial.print(DIGITAL_MODE);                                         
  Serial.print("    |     Analog Level = ");
  Serial.println(ANALOG_MODE);

  delay(500);                                                         //Delay between the readings
}
