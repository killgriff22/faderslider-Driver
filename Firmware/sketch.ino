

#include "Grove_I2C_Motor_Driver.h"

// default I2C address is 0x0f
#define I2C_ADDRESS 0x0f
#define Button0 2 
#define Button1 3
int targetValue = 500;
int paddingA =5;
int paddingB =10;
int maxDelta = 0;
int minDelta = 500;
int timer = 500;
bool lastB0Val = false;
bool lastB1Val = false;
void setup() {
  Motor.begin(I2C_ADDRESS);
  Motor.speed(MOTOR1, 255); // ensure we take control of the motor.
  //Motor.stop(MOTOR1);
  pinMode(A0, INPUT);
  pinMode(A3, OUTPUT);
  pinMode(Button0, INPUT);
  pinMode(Button1, INPUT);
  Serial.begin(115200); // fast.
}

void loop() {
  int potValue = analogRead(A0);
  bool button0Val = digitalRead(Button0);
  bool button1Val = digitalRead(Button1);
  float potDelta = (potValue-targetValue); // how far the actual value is from our desired value
  // logic to use toggle-on-press - also debounce.
  if (button0Val and not lastB0Val){
    lastB0Val = true;
  } else if (not button0Val) {
    lastB0Val = false;
  }
  if (button1Val and not lastB1Val){
    lastB1Val = true;
  } else if (not button1Val) {
    lastB1Val = false;
  }
  Serial.print("button0Val:"); 
  Serial.print(button0Val);
  Serial.print(",button1Val:"); 
  Serial.print(button1Val);
  Serial.print(",potDelta:"); 
  Serial.println(potDelta);

  if (!button1Val) {
    targetValue = potValue;
    return;
  }
  float floatSpeed = (potDelta/1024)*25;
  int intSpeed = (int)floatSpeed;

  if ((abs(potDelta) < paddingB)) {
    Motor.speed(MOTOR1, -intSpeed);
    digitalWrite(A3, HIGH);
  }
  if ((abs(potDelta) > paddingB)) {
    digitalWrite(A3, LOW);
    Motor.speed(MOTOR1, intSpeed);
  }
    if (button0Val) {
    digitalWrite(A3, LOW);
  }
}

// End of file
