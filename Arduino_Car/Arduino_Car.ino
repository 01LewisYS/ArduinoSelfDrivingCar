/*
Note: 
1) Ultrasonic Sensor (US)
2) If distance to the front of car is <= 40cm, then it's considered no space in front.
3) If distance to the left/right of car is <=35cm, then it's considered no space in left/right. There will be 1 exception 
   to this where <=25cm considers as no space (line 71).
*/

#include <NewPing.h> 
#define SONAR_NUM 3 //Total no. of US

NewPing sonar[SONAR_NUM] = { //Sensor object array.
  NewPing(11, 10, 400), //US no.0: detects front distance, where NewPing(trig, echo pin, max distance (cm) that can detect).
  NewPing(5, 6, 400),  //US no.1: detects right distance.
  NewPing(13, 12, 400) //US no.2: detects left distance.
};

int ena = 3; //Enable A controls speed of both left wheels (Output A).
int enb = 9; //Enable B controls speed of both right wheels (Output B).
int in1 = 2; //Controls output Motor terminal 1
int in2 = 4; //Controls output Motor terminal 2
int in3 = 7; //Controls output Motor terminal 3
int in4 = 8; //Controls output Motor terminal 4
int distanceF; //Distance from middle US to object in front.
int distanceR; //Distance from right US to object on the right.
int distanceL; //Distance from left US to object on the left.
int carVelocity = 175; //Analog value of motor's/wheel's speed. carVelocity =0 (0V) is stationary, and carVelocity = 255 (5V) is max speed.
int distanceArray[3]; //[distanceF, distanceR, distanceL], where 1st element = distanceF; 2nd element = distanceR, 3rd element = distanceL.

//Variables used for a randomizer function that allows the car to think like a human, to turn left/right when both turns can be done without collisions.
int distanceX;
String directionX;
int randomNum;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  delay(2500); //Car waits for 2.5s before car starts moving into action.
}

void loop() {
  // put your main code here, to run repeatedly:
  for (uint8_t i = 0; i < SONAR_NUM; i++) {//Loop through each sensor and display results.
    delay(50); //Wait 50ms between pings (29ms should be the shortest delay between pings).
    distanceArray[i] = sonar[i].ping_cm(); //E.g. sonar[0].ping_cm() measures the distance between middle US (US no.0) and object in front, etc.
  }
  distanceF = distanceArray[0];
  distanceR = distanceArray[1];
  distanceL = distanceArray[2];
  Serial.println("F: " + String(distanceF)); //Front distance
  Serial.println("R: " + String(distanceR)); //Right distance
  Serial.println("L: " + String(distanceL)); //Left distance

  /* Arduino thinks like a person, making randomised decision whether to turn left/right first when 
  no space in front, but space is available on the left AND right. */
  randomNum = random(2); //random function generates a whole number from 0 to 1 inclusive, where 0=right & 1=left.
  if (randomNum == 0){
    distanceX = distanceR;
    directionX = "right";
  }
  else{
    distanceX = distanceL;
    directionX = "left";
  }
  if (distanceF>0 && distanceF<=40 && distanceL>25 && distanceR>25){ 
    //exception: If distance to the left/right of car is <=25cm, then it's considered no space in left/right.
    //The following myString.compareTo(myString2) function returns the value 0 if 2 strings are equal.
    if (directionX.compareTo("right")==0){ //If random generator stored "right" into directionX, car will choose to turn right.
      turnRight();
    }
    else{
      turnLeft();
    }
  }

  //Go forward if front got space AND road is narrow enough (no obstacles left and right).
  if ((distanceF>40 || distanceF==0) && distanceR>35 && distanceL>35){
    goForward();
  }

  //If left and front of car has no space, turn right.
  if (distanceL<=35 && (distanceF>0 && distanceF<=40) && distanceR>35){
    turnRight();
  }

  //If right and front of car has no space, turn left.
  if (distanceR<=35 && (distanceF>0 && distanceF<=40) && distanceL>35){
    turnLeft();
  }

  //If car approaches obstacle on the left AND there's extra space on the right side, turn right.
  if (distanceL<=35 && distanceR>35){
    turnRight();
  }

  //If car approaches obstacle on the right AND there's extra space on the left side, turn left
  if (distanceR<=35 && distanceL>35){
    turnLeft();
  }

  /* If car reaches a dead end (US detects that the distance of front, left, right have no space), 
  car rotates 180 degrees on the spot so that it can exit the dead end. */
  if (distanceF>0 && distanceF<=40 && distanceR<=35 && distanceL<=35){
    rotateAround();
  }
}

//All wheels turn clockwise to move forward.
void goForward(){ 
  analogWrite(ena, carVelocity);
  analogWrite(enb, carVelocity);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

//All wheels turn anticlockwise to move backward.
void goBackward(){ //
  analogWrite(ena, carVelocity);
  analogWrite(enb, carVelocity);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

//Both left wheels go forward, but both right wheels go backward: car turns right.
void turnRight(){
  analogWrite(ena, carVelocity);
  analogWrite(enb, carVelocity);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

//Both right wheels go forward, but both left wheels go backward: car turns left.
void turnLeft(){
  analogWrite(ena, carVelocity);
  analogWrite(enb, carVelocity);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

//All wheels stop moving to keep car stationary. [Optional function to use]
void brakeStop(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

//Car rotates clockwise for 750ms: car rotates 180 degrees.
void rotateAround(){
  analogWrite(ena, carVelocity);
  analogWrite(enb, carVelocity);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(750);
}
