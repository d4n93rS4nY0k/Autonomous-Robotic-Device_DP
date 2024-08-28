#define PIN 7
#define NUMPIXELS 16 

#include <Servo.h>

long duration1, duration2, duration3, forward, right, grey, green_l, green_r, red, speed_eva_l = 0, speed_eva_r = 0;
unsigned long time_red = 0, eva_time, start_time = millis(), back_time = millis();
bool evacuation = false, eva = false;
char disp;
float error = 0, P = 0, I = 0, D = 0, PIDvalue = 0, count = 0;
float previousError = 0, previousI = 0;
const int power = 100, power_eva = 15;
const int iniMotorPower = 150;
const int adj = 1;
float adjTurn = 8;
float Kp = 3;
float Ki = 0;
float Kd = 7;

Servo leftFServo;
Servo rightFServo;
Servo leftBServo;
Servo rightBServo;

void setup() {
  Serial.begin(9600);

  rightFServo.attach(11);
  rightBServo.attach(13);
  leftFServo.attach(10);
  leftBServo.attach(12); 
  
}

void loop() {
  message();

  if (evacuation == true) {
    grey = 0;
    eva = false;
  }

  if ((grey == 0) and (eva == false)) {
    P = error;
    I = I + error;
    D = error - previousError;
    PIDvalue = (Kp * P);
    previousError = error;

    PidMove();

  
    if (millis() - time_red < 1500){
      red = 0;
    }
    if (red == 1){
      delay(1000);
      leftFServo.write(90);
      leftBServo.write(90);
      rightFServo.write(90);
      rightBServo.write(90);
      delay(5000);
      time_red = millis();
    }
  } else {
    delay(2);
    message();
  }
}

void PidMove(){
  int leftFMotorSpeed = (1500 + iniMotorPower - PIDvalue);
  int rightFMotorSpeed = 1500 - iniMotorPower * adj - PIDvalue;
  int leftBMotorSpeed = (1500 + iniMotorPower - PIDvalue);
  int rightBMotorSpeed = 1500 - iniMotorPower * adj - PIDvalue;
  leftFServo.writeMicroseconds(leftFMotorSpeed );
  rightFServo.writeMicroseconds(rightFMotorSpeed);
  leftBServo.writeMicroseconds(leftBMotorSpeed);
  rightBServo.writeMicroseconds(rightBMotorSpeed);
}

void Detour(){
  int frw = 900, trn = 1300;
  leftFServo.write(80);
  leftBServo.write(80);
  rightFServo.write(40);
  rightBServo.write(40);
  delay(trn);
  leftFServo.write(110);
  leftBServo.write(110);
  rightFServo.write(70);
  rightBServo.write(70);
  delay(frw);
  leftFServo.write(145);
  leftBServo.write(145);
  rightFServo.write(105);
  rightBServo.write(105);
  delay(trn);
  leftFServo.write(110);
  leftBServo.write(110);
  rightFServo.write(70);
  rightBServo.write(70);
  delay(frw*1.5);
  leftFServo.write(145);
  leftBServo.write(145);
  rightFServo.write(105);
  rightBServo.write(105);
  delay(trn);
  leftFServo.write(110);
  leftBServo.write(110);
  rightFServo.write(70);
  rightBServo.write(70);
  delay(frw);
  leftFServo.write(80);
  leftBServo.write(80);
  rightFServo.write(40);
  rightBServo.write(40);
  delay(trn);
}

void message() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil(',');
    green_r = input.toInt();
    
    input = Serial.readStringUntil(',');
    green_l = input.toInt();
    
    input = Serial.readStringUntil(',');
    error = input.toInt();
    
    input = Serial.readStringUntil(',');
    red = input.toInt();

    input = Serial.readStringUntil('.');
    grey = input.toInt();
    
  }
}
