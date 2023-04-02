#include <Servo.h>          //Servo motor library. This is standard library
#include <SoftwareSerial.h>
//Fire Extinguishing Robot with Obstacle Detection
const int rightSideForward = 4;
const int rightSideBackward = 5;
const int leftSideForward = 6;
const int leftSideBackward = 7;

//sensor pins
#define echoPin 11 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 12 //attach pin D3 Arduino to pin Trig of HC-SR04

//long duration; // variable for the duration of sound wave travel
//int distance; // variable for the distance measurement

int forwardValue, leftValue, rightValue;
#define forwardFlame A1
#define leftFlame A3
#define rightFlame A2

#define relaySignalPin 2

#define maximum_distance 200
Servo servo_motor; //our servo name

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

int redLed = 8;
int buzzer = 13;
int smokeA0 = A0;
// Your threshold value
int smokeThreshold = 40;
bool fire = false;

unsigned long previousTime = 0;
String messageSent;

const String phoneNumber = "+2348158943689";  //subject to change phoneNumber

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);//Begin serial communication with Arduino and Serial
  servo_motor.attach(10); //our servo pin
  //Declaring Outputs
  declareOutputs();
  //Declaring Inputs
  declareInputs();
  digitalWrite(relaySignalPin, HIGH);
  //simModuleInitializiation();
}

void loop() {
if(!fireDetected()){
  forward(30);
  if(obstacle()){
    backward(500);
    if(scanObstacle() == "right"){
      turnRight(500);
    } else{
      turnLeft(500);
    }
  }
} else {
  digitalWrite(redLed, HIGH);
  digitalWrite(buzzer, HIGH);
  //send SMS
  SendSMS();
  if(getDirection() == "front"){
    backward(200);
    delay(1000);
    forward(300);
    delay(1000);
    sprayWater();
    delay(2000);
    
  } else if(getDirection() == "right"){
    backward(300);
    delay(1000);
    turnRight(500);
    delay(1000);
    forward(100);
    delay(1000);
    sprayWater();
    delay(2000);
    
  } else if(getDirection() == "left"){
    backward(300);
    delay(1000);
    turnLeft(500);
    delay(1000);
    forward(100);
    delay(1000);
    sprayWater();
    delay(2000);
  }
  digitalWrite(redLed, LOW);
  digitalWrite(buzzer, LOW);  
}


}

void declareOutputs() {
  pinMode(leftSideForward, OUTPUT);
  pinMode(leftSideBackward, OUTPUT);
  pinMode(rightSideForward, OUTPUT);
  pinMode(rightSideBackward, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relaySignalPin, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
}

void declareInputs() {
  pinMode(rightFlame, INPUT);
  pinMode(forwardFlame, INPUT);
  pinMode(leftFlame, INPUT);
  pinMode(smokeA0, INPUT);
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
}

String scanObstacle(){
//  int f = lookFront();
  int r = lookRight();
  delay(500);
  int l = lookLeft();
  delay(500);
  if(r > l){
    return "right";
  }else if(l > r){
    return "left";
  }
}

bool obstacle(){
  if( lookFront() <= 30 ){
    return true;
  }
  else {
    return false;
  }
}

int lookRight() {
  int dist;
  servo_motor.write(0);
  delay(1000);
  dist = distance_ ();
  delay(1000);
  servo_motor.write(90);
  return dist;
}

int lookFront() {
  int dist;
  servo_motor.write(90);
  delay(1000);
  dist = distance_ ();
  return dist;
}

int lookLeft() {
  int dist;
  servo_motor.write(180);
  delay(1000);
  dist = distance_ ();
  delay(1000);
  servo_motor.write(90);
  return dist;
}

String getDirection() {
  forwardValue = digitalRead(forwardFlame);
  rightValue = digitalRead(rightFlame);
  leftValue = digitalRead(leftFlame);

  if (forwardValue == 1) {
    return "front";
  } else if (rightValue == 0) {
    return "right";
  } else if (leftValue == 0) {
    return "left";
  }
}

void forward(int moveTime) {
  digitalWrite(leftSideForward, HIGH);
  digitalWrite(rightSideForward, HIGH);
  delay(moveTime);
  stop_();
}

void backward(int moveTime ) {
  digitalWrite(leftSideBackward, HIGH);
  digitalWrite(rightSideBackward, HIGH);
  delay(moveTime);
  stop_();
}

void turnLeft(int moveTime) {
  digitalWrite(rightSideForward, HIGH);
  digitalWrite(leftSideForward, LOW);
  delay(moveTime);
  stop_();
}

void turnRight(int moveTime) {
  digitalWrite(rightSideForward, LOW);
  digitalWrite(leftSideForward, HIGH);
  delay(moveTime);
  stop_();
}

void stop_() {
  digitalWrite(leftSideForward, LOW);
  digitalWrite(leftSideBackward, LOW);
  digitalWrite(rightSideForward, LOW);
  digitalWrite(rightSideBackward, LOW);
}

bool gasDetected () {
  int smokeValue = analogRead(smokeA0);

  Serial.print("Pin A0: ");
  Serial.println(smokeValue);
  // Checks if it has reached the threshold value
  if (smokeValue > smokeThreshold) {
    return true;
  }
  else
  {
    return false;
  }

}

bool flameDetected() {
  forwardValue = digitalRead(forwardFlame);
  rightValue = digitalRead(rightFlame);
  leftValue = digitalRead(leftFlame);

  if (forwardValue == 1) {
    return true;
  } else if (leftValue == 0) {
    return true;
  } else if (rightValue == 0) {
    return true;
  } else {
    return false;
  }

}

bool fireDetected() {
  if ( gasDetected () && flameDetected() ) {
    return true;
  } else {
    return false;
  }
}

void simModuleInitializiation() {
  //  Serial.println("Initializing....");
  initModule("AT", "OK", 1000);
  initModule("ATE1", "OK", 1000);
  initModule("AT+CPIN?", "READY", 1000);
  initModule("AT+CMGF=1", "OK", 1000);

  //  initModule("AT+CMGD=1,4", "OK", 1000);
  initModule("AT+CNMI=1,2,0,0,0", "OK", 1000);
  delay(2000);
}

void initModule(String cmd, char *res, int t) { // function to initialize sim module for SMS setup
  while (1) {
    //    Serial.println(cmd);
    Serial1.println(cmd);
    delay(100);
    while (Serial1.available() > 0) {
      if (Serial1.find(res)) {
        Serial.println(res);
        delay(t);
        return;
      }
      else {
        Serial.println("Error");
      }
    }
    delay(t);
  }
}

void SendSMS() { //Send SMS function
  delay(100);
  Serial1.println("AT");
  delay(500);
  Serial1.println("AT+CMGF=1");
  delay(500);
  //  phoneNumber = phoneNumber.substring(0, 14);
  Serial.println("---");
  Serial.println(phoneNumber);
  Serial1.print("AT+CMGS=");
  Serial1.print('"');
  Serial1.print(phoneNumber);    //mobile no. for SMS alert
  Serial1.println('"');
  delay(500);
  Serial1.println("\n Fire Alert!!!, pump is now activated");
  delay(500);
  Serial1.write(26);
  delay(500);
}//end sendSMS

void sprayWater() {
  digitalWrite(relaySignalPin, LOW);
  delay(3000);
  digitalWrite(relaySignalPin, HIGH);
}

int distance_ () {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor-
//  Serial.print(distance);
//  Serial.println(" cm");
  return distance;
}
