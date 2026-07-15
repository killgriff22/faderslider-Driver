

#include "Grove_I2C_Motor_Driver.h"

// default I2C address is 0x0f
#define I2C_ADDRESS 0x0f
#define Button0 2 
#define Button1 3
#define RELAY_PIN 7
#define _DEBUG false
int c = 500;
int paddingA =5;
int paddingB =10;
int targetValue = 500;
int calibratemin = 10;
int calibratemax = 1000;
int potValue = 0;
bool lastb0 = false;
char buf[6];
void setup() {
  Motor.begin(I2C_ADDRESS);
  Motor.speed(MOTOR1, 255); // ensure we take control of the motor.
  //Motor.stop(MOTOR1);
  pinMode(A0, INPUT);
  pinMode(Button0, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  if (_DEBUG){
    Serial.begin(115200); // fast.
  } else {
    Serial.begin(9600); // less fast.
  }
}

void move_to_value(bool DEBUG = false) {
  potValue = analogRead(A0);
  float potDelta = (potValue-targetValue); // how far the actual value is from our desired value
  float floatSpeed = (potDelta/1024)*25;
  int intSpeed = (int)floatSpeed;
  if (DEBUG){
    Serial.print("potValue:"); 
    Serial.print(potValue);
    Serial.print(",intSpeed:");
    Serial.print(intSpeed);
    Serial.print(",floatSpeed:");
    Serial.print(floatSpeed);
    Serial.print(",potDelta:"); 
    Serial.println(potDelta);
  }
  if (abs(floatSpeed) <= .49){  
    Motor.stop(MOTOR1);
    digitalWrite(RELAY_PIN, HIGH);
    return;
  }
  if (abs(floatSpeed) > 0 and intSpeed == 0){
    intSpeed = (floatSpeed > 0) - (floatSpeed < 0);
  }
  if ((abs(potDelta) < paddingA)) {
    digitalWrite(RELAY_PIN, HIGH);
  }
  if ((abs(potDelta) > paddingA)) {
    digitalWrite(RELAY_PIN, LOW);
    Motor.speed(MOTOR1, intSpeed);
  }
}


void readSerial(){
    if (Serial.available()) {
    String teststr = Serial.readString();  //read until timeout
    teststr.trim();
    //Serial.println(teststr);
    if (teststr == "R0") {
      Serial.print(potValue);
    }
    if (teststr == "W0") {
        digitalWrite(RELAY_PIN, HIGH);
        Serial.print("#");
        while (!(Serial.available())){}
        int number = Serial.parseInt();
        targetValue = number*10;
        //sprintf(buf, "%03s", targetValue);
        Serial.print(targetValue); // Output: "Temp:  24.68 C"
        Serial.flush();
    }
  }
  Serial.flush();
}
bool toggleshit = false;
bool lasttoggleshit = false;

bool readButtons(){
  bool button0val = digitalRead(Button0);
  if (_DEBUG){
    Serial.print("last0:");
    Serial.print(lastb0);
    Serial.print(",0:");
    Serial.print(button0val);
    Serial.print(",toggle:");
    Serial.print(toggleshit);
    Serial.print(",lasttoggle:");
    Serial.println(lasttoggleshit);
  }
  if (button0val and not lastb0) {
      lastb0 = true;
    toggleshit = not toggleshit;
    // on press
  } else if ((not button0val) and lastb0){
    lastb0 = false;

    // on release
  }
  if (toggleshit){
    lasttoggleshit = true;
    digitalWrite(RELAY_PIN, HIGH);
    return true;
  } 
  if (not toggleshit and lasttoggleshit) {
    Serial.print("0F");
    while (!Serial.available() > 0) {}
    targetValue = analogRead(A0);
    Serial.print(targetValue);
    digitalWrite(RELAY_PIN, LOW);
    toggleshit = false;
    lasttoggleshit = false;
  }
  return false;
}


void loop() {
  bool ret = readButtons();
  if (ret){return;}
  move_to_value(false);
  readSerial();
}

// End of file
