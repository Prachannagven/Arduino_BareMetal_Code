#define DATA_PIN 3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(DATA_PIN, INPUT);
}

void loop() {
  bool STATE = DETECT();
  if(STATE){
    Serial.println("Nothing Detected");
  }
  else if(!STATE){
    Serial.println("Object Detected");
  }
  delay(500);
}

bool DETECT(){
  bool STATE = digitalRead(DATA_PIN);
  return STATE;
}