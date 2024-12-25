#define DATA_PIN 3
#define CLK_PIN 5


void setup() {
  Serial.begin(9600);

  pinMode(DATA_PIN, INPUT);
  pinMode(CLK_PIN, OUTPUT);

  long RAW_DATA = READ_DATA();
  SET_GAIN(128);

  long DATA = CONVERT_TWOS_COMPLEMENT(RAW_DATA);
  
  Serial.println(DATA, BIN);
}

void loop() {
  // put your main code here, to run repeatedly:

}

long READ_DATA(){
  long DATA = 0;
  while(digitalRead(DATA_PIN) == HIGH);

  for(int i = 0; i < 24 ; i++){
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(1);
    DATA = (DATA << 1) | digitalRead(DATA_PIN);
    /*
    if(digitalRead(DATA_PIN) == HIGH){
      DATA = DATA | (1 << (24-i));
    }
    */
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(1);
  }

  return DATA;
}

void SET_GAIN(volatile int GAIN){
  if(GAIN == 128){
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(1);

    Serial.println("Gain set to 128");
  }
  else if(GAIN == 32){
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(1);

    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(1);

    Serial.println("Gain set to 32");
  }
  else if(GAIN == 64){
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(1);

    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(1);

    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(1);

    Serial.println("Gain set to 64");
  }
  else{
    Serial.println("Not a valid gain, please enter only 128, 64 or 32");
  }
}

long CONVERT_TWOS_COMPLEMENT(volatile long RAW_DATA){
  long CLEANED_DATA = 0;
  if(RAW_DATA & (1UL << 23)){
    CLEANED_DATA = RAW_DATA - (1UL << 24);
  }
  else{
    CLEANED_DATA = RAW_DATA;
  }

  return CLEANED_DATA;
}