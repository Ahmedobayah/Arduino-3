/*
TB6612FNG_4WD_robot_bluetooth_controlled
 - Allows you to control a mobile robot via bluetooth
 - Tested with Arduino Pro Micro
 - Android application - https://play.google.com/store/apps/details?id=pl.mobilerobots.vacuumcleanerrobot&hl=pl
 www: http://www.mobilerobots.pl/

 Connections:
 Bluetooth -> Arduino pro Micro
 Rx - Tx 
 Tx - Rx 
 
 TB6612FNG Dual Motor Driver -> Arduino pro Micro
 AIN1 - 4
 AIN2 - 7
 BIN1 - 8
 BIN2 - 9
 PWMA - 5
 PWMB - 6
 STBY - Vcc
 VMOT - motor voltage (4.5 to 13.5 V) - 12V from LiPol
 Vcc - logic voltage (2.7 to 5.5) - 5V from Breadboard Power Supply
 
 TB6612FNG Dual Motor Driver -> DC Motors
 AO1 - drive motor A
 A02 - drive motor A
 B01 - steering motor B
 B02 - steering motor B
 
 IR Digital Sensors -> Arduino pro Micro
 VIN - 5V
 GND - GND
 VOUT1 - 18
 VOUT2 - 19
 
 */
#include <Wire.h>
#include <math.h>

/*TB6612FNG Dual Motor Driver Carrier*/
const int MotorA_AIN1 = 4; // control Input AIN1 - motor A
const int MotorA_AIN2 = 7; // control Input AIN2 - motor A
const int MotorB_BIN1 = 8; // control Input BIN1 - motor B
const int MotorB_BIN2 = 9; // control Input BIN2 - motor B
// IR SENSORS
const int IRsensorFront = 18; // Front sensor
const int IRsensorRear = 19;  // Rear sensor
// SDA = 2
// SCL = 3
const int pwm_A = 5; // pwm output - motor A
const int pwm_B = 6; // pwm output - motor B
long pwmLvalue = 255;
long pwmRvalue = 255;
byte pwmChannel;
const char startOfNumberDelimiter = '<';
const char endOfNumberDelimiter = '>';

void setup(){
  Serial1.begin(9600);// default baudrate for HC-06, HC-05, PIN:1234
  // IR SENSORS
  pinMode(IRsensorFront, INPUT);  // declare Front IR Sensor as input
  pinMode(IRsensorRear, INPUT);   // declare Rear IR Sensor as input

  //Setup Channel A - Drive Motor
  pinMode(MotorA_AIN1, OUTPUT); //Initiates Motor Channel A1 pin
  pinMode(MotorA_AIN2, OUTPUT); //Initiates Motor Channel A2 pin

  //Setup Channel B - Steering Motor
  pinMode(MotorB_BIN1, OUTPUT); //Initiates Motor Channel B1 pin
  pinMode(MotorB_BIN2, OUTPUT); //Initiates Motor Channel B2 pin
  //Wire.begin();
}// void setup()

void loop(){ 
  //readIRsensors();
  if (Serial1.available()) {
    processInput();
  }
}// void loop()

void readIRsensors(){
  int IR_FrontValue = analogRead(IRsensorFront);  // read input value from front IR sensor
  int IR_BackValue = analogRead(IRsensorRear);    // read input value from rear IR sensor
  if(IR_FrontValue>400){ 
    goBackwad(255);
    delay(200);
    stop_Robot();   
  }
  else if (IR_BackValue>400){
    goForward(255);
    delay(200);
    stop_Robot();   
  }
}// void readIRsensors()

void processInput (){
  static long receivedNumber = 0;
  static boolean negative = false;
  byte c = Serial1.read ();

  switch (c){
  case endOfNumberDelimiter:
    if (negative)
      SetPWM(- receivedNumber, pwmChannel);
    else
      SetPWM(receivedNumber, pwmChannel);

    // fall through to start a new number
  case startOfNumberDelimiter:
    receivedNumber = 0;
    negative = false;
    pwmChannel = 0;
    break;

  case 'g': // robot GO!
    go_Robot();
    break;

  case 'f': // Go FORWARD
    goForward(255);
    //Serial.println("forward");
    break;

  case 'b': // Go BACK
    goBackwad(255);
    //Serial.println("backward");
    break;

  case 'r':
    turnRight(255);
    break;

  case 'l':
    turnLeft(255);
    break;

  case 'c': // Top Right
    turnRightForward(255);
    break; 

  case 'd': // Top Left
    turnLeftForward(255);
    break;  

  case 'e': // Bottom Right
    turnRightBack(255);
    break; 

  case 'h': // Bottom Left
    turnLeftBack(255);
    break;  

  case 's':
    stop_Robot();
    break;

  case 'x':
    pwmChannel = 1; // pwm_A
    break;
  case 'y': // pwm_B
    pwmChannel = 2;
    break;

  case '0' ... '9':
    receivedNumber *= 10;
    receivedNumber += c - '0';
    break;

  case '-':
    negative = true;
    break;
  } // end of switch
} // void processInput ()

void motorsControl(int rightMotorsSpeed, int leftMotorsSpeed){
  if(rightMotorsSpeed>0){
    digitalWrite(MotorA_AIN1, LOW); 
    digitalWrite(MotorA_AIN2, HIGH); 
  }
  else if(rightMotorsSpeed<0){
    digitalWrite(MotorA_AIN1, HIGH); 
    digitalWrite(MotorA_AIN2, LOW); 
  }  
  if(leftMotorsSpeed>0){
    digitalWrite(MotorB_BIN1, LOW); 
    digitalWrite(MotorB_BIN2, HIGH); 
  }
  else if(leftMotorsSpeed<0){
    digitalWrite(MotorB_BIN1, HIGH); 
    digitalWrite(MotorB_BIN2, LOW); 
  } 
  analogWrite(pwm_A, abs(rightMotorsSpeed));
  analogWrite(pwm_B, abs(leftMotorsSpeed));
}

void go_Robot(){
  digitalWrite(MotorA_AIN1, LOW);  
  digitalWrite(MotorA_AIN2, HIGH); 
  analogWrite(pwm_A, pwmRvalue);
}// void go_Robot()

void goForward(int mspeed){
  motorsControl(mspeed, mspeed);
}// void goForward(int mspeed, int time_ms)

void goBackwad(int mspeed){
  motorsControl(-mspeed, -mspeed);
}// void goBackwad(int mspeed, int time_ms)

void turnRight(int mspeed){
  motorsControl(-mspeed, mspeed);
}// void goBackwad(int mspeed, int time_ms)

void turnLeft(int mspeed){
  motorsControl(mspeed, -mspeed);
}// void turnLeft(int mspeed)

void turnRightForward(int mspeed){
  motorsControl(mspeed*0.4, mspeed);
}

void turnLeftForward(int mspeed){
  motorsControl(mspeed, mspeed*0.4);
}

void turnRightBack(int mspeed){
  motorsControl(-mspeed*0.4, -mspeed);
}// void goBackwad(int mspeed, int time_ms)

void turnLeftBack(int mspeed){
  motorsControl(-mspeed, -mspeed*0.4);
}// turnLeftBack(int mspeed)

void stopRobot(int delay_ms){
  analogWrite(pwm_A, 0);
  analogWrite(pwm_B, 0);
  delay(delay_ms);
}// void stopRobot(int delay_ms)

void stop_Robot(){
  analogWrite(pwm_A, 0);
  analogWrite(pwm_B, 0);
}// void stopRobot()

void SetPWM (const long pwm_num, byte pwm_channel){
  if(pwm_channel==1){ 
    analogWrite(pwm_A, pwm_num);
    pwmRvalue = pwm_num;
  }
  else if(pwm_channel==2){ 
    analogWrite(pwm_B, pwm_num);
    pwmLvalue = pwm_num;
  }
}// void SetPWM (const long pwm_num, byte pwm_channel)

