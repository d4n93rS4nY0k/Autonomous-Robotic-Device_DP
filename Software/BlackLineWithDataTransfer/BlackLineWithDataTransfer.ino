#define PIN 7
#define NUMPIXELS 16 

#include <Servo.h>
#include <SPI.h>
#include <RF24.h>
#include "printf.h"

RF24 radio(9,10);

long duration1, duration2, duration3, forward, right;
unsigned long start_time = millis(), back_time = millis();
float error = 0, P = 0, I = 0, D = 0, PIDvalue = 0, count = 0;
float previousError = 0, previousI = 0;
const int power = 100;
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
  while(!Serial);
  Serial.begin(9600);
******************************************************************************************************************************************************************************
***************************************************************************  RADIO RECEIVE CONFIGURATION SETTINGS  ***********************************************************
******************************************************************************************************************************************************************************
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  const uint64_t pipe = 0xE8E8F0F0E1LL;
  radio.openReadingPipe(1, pipe);
  radio.enableDynamicPayloads();
  radio.powerUp();
  printf_begin(); 
  radio.printDetails();
  radio.startListening();
******************************************************************************************************************************************************************************
***************************************************************************  SERVO ATTACH  ***********************************************************************************
******************************************************************************************************************************************************************************

  rightFServo.attach(11);
  rightBServo.attach(13);
  leftFServo.attach(10);
  leftBServo.attach(12); 
  
}

void loop() {
    PidMove();
******************************************************************************************************************************************************************************
***************************************************************************  DATA RECEIVE FROM RPi  **************************************************************************
******************************************************************************************************************************************************************************
    radio.startListening();
    char receivedMessage[32] = {0};
    if(radio.available()){
      radio.read(receivedMessage, sizeof(receivedMessage));
      Serial.println(receivedMessage);
      radio.stopListening();
    }
    delay(2000);
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
