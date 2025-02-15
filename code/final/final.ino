#include <Servo.h>
#include <Wire.h>
#include <I2CEncoder.h>
#include <uSTimer2.h>

// Authors: Danny, Tin, Daniel
// Prepared for: MSE 2202B, Dr. Naish
// School: The University of Western Ontario

// --------------------------------------------------------------
// ------------------------- !VARIABLES -------------------------
// --------------------------------------------------------------


// -------------------- MOTORS --------------------

// 1500ms = Neutral
// 2500ms = Fast forward
// 500ms = Fast Reverse
// 200ms = Brake

Servo servo_TopMotor;
Servo servo_RightMotor;
Servo servo_LeftMotor;


I2CEncoder encoder_TopMotor;
I2CEncoder encoder_LeftMotor;
I2CEncoder encoder_RightMotor;


// -------------------- SENSORS --------------------
const int TOP_MOTOR_PIN = 4;
const int RIGHT_MOTOR_PIN = 2;
const int LEFT_MOTOR_PIN = 3;

const int FRONT_BOTTOM_LEVER_SWITCH_PIN = 4;
const int FRONT_TOP_LEVER_SWITCH_PIN = 5;

const int ULTRASONIC_IN_PIN_FRONT = 52;
const int ULTRASONIC_OUT_PIN_FRONT = 53;

const int ULTRASONIC_IN_PIN_BACK = 48;
const int ULTRASONIC_OUT_PIN_BACK = 49;

// -------------------- VARIABLES --------------------

unsigned long echo_time[2];//0 is front timer, 1 is back timer

unsigned int Left_Motor_Speed;
unsigned int Right_Motor_Speed;
unsigned int Right_Motor_Stop = 1500;
unsigned int Left_Motor_Stop = 1500;
long leftEncoderStopTime = 0;
long rightEncoderStopTime = 0;
boolean loopStarted = false;// start loop for turning
int Left_Motor_Offset = 0;
int Right_Motor_Offset = 25;

unsigned long time_previous = 0; // Used for time functions, do not change
unsigned long time_elapsed = 0; // Used for time functions, do not change
boolean can_start_waiting = false; // Used for time functions, do not change



// -------------------- PID --------------------
const float setPoint = 700; // 15cm
const float dSensors = 2700;
const float speedConstant = 200;

// -------------------- STAGE COUNTER --------------------
// NOTE: Stage 0 is reserved for debugging


unsigned int stage = 0;


// ******************************************************************
// ************************* PROGRAM !SETUP *************************
// ******************************************************************
void setup() {

  Wire.begin();
  Serial.begin(9600);

  // Set-up motors

  pinMode(TOP_MOTOR_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  servo_LeftMotor.attach(LEFT_MOTOR_PIN);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
  servo_RightMotor.attach(RIGHT_MOTOR_PIN);


  // Set-up buttons

  pinMode(FRONT_TOP_LEVER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(FRONT_BOTTOM_LEVER_SWITCH_PIN, INPUT_PULLUP);

  digitalWrite(FRONT_TOP_LEVER_SWITCH_PIN, HIGH);
  digitalWrite(FRONT_BOTTOM_LEVER_SWITCH_PIN, HIGH);

  // Set-up ultrasonic
  pinMode(ULTRASONIC_OUT_PIN_FRONT, INPUT);
  pinMode(ULTRASONIC_IN_PIN_FRONT, OUTPUT);

  pinMode(ULTRASONIC_OUT_PIN_BACK, INPUT);
  pinMode(ULTRASONIC_IN_PIN_BACK, OUTPUT);



  //Set up encoder
  encoder_LeftMotor.init(1.0 / 3.0 * MOTOR_393_SPEED_ROTATIONS, MOTOR_393_TIME_DELTA);
  encoder_LeftMotor.setReversed(false); // adjust for positive count when moving forward
  encoder_RightMotor.init(1.0 / 3.0 * MOTOR_393_SPEED_ROTATIONS, MOTOR_393_TIME_DELTA);
  encoder_RightMotor.setReversed(true); // adjust for positive count when moving forward

  encoder_LeftMotor.zero();
  encoder_RightMotor.zero();
  encoder_TopMotor.zero();
}

// *****************************************************************
// ************************* PROGRAM !LOOP *************************
// *****************************************************************
void loop() {

  switch (stage) {

    // ==================== CASE 1-10 ====================

    case 0:
      // RESERVED FOR TESTING, PASTE CODE HERE AND SET STAGE = 0
      turnLeftAngle(360);
            stage++;
      break;

    case 1:
      // Case status: DONE by Daniel
      // Start case: Robot is in the middle of the room
      moveForward(300);
      if(hitWall()){
        backUp();
        stage = 30;
      }
      break;
      // End case: Robot is parallel to the wall

    case 2:
      // Start case: Wall is in front of robot, robot is parallel to it
          
      // End case: Robot is hit the wall
      break;

    case 3:
      break;

    case 4:
      break;

    case 5:
      break;

    case 6:
      break;

    case 7:
      break;

    case 8:
      break;

    case 9:
      break;

    case 10:
      break;


    // ==================== CASE 11-20 ====================

    case 11:
      break;

    case 12:
      break;

    case 13:
      break;

    case 14:
      break;

    case 15:
      break;

    case 16:
      break;

    case 17:
      break;

    case 18:
      break;

    case 19:
      break;

    case 20:
      break;

    // ==================== CASE 21-30 ====================

    case 21:
      break;

    case 22:
      break;

    case 23:
      break;

    case 24:
      break;

    case 25:
      break;

    case 26:
      break;

    case 27:
      break;

    case 28:
      break;

    case 29:
      break;


  case 30:
    break;
  }

}


// --------------------------------------------------------------
// ------------------------- !FUNCTIONS -------------------------
// --------------------------------------------------------------



// -------------------- SENSOR FUNCTIONS --------------------

// Ping ultrasonic
// Send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec

int frontPing() {
  //Front ultrasonic
  digitalWrite(ULTRASONIC_IN_PIN_FRONT, HIGH);
  delayMicroseconds(10); //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ULTRASONIC_IN_PIN_FRONT, LOW);

  unsigned long ping_time;
  echo_time[0] = pulseIn(ULTRASONIC_OUT_PIN_FRONT, HIGH, 10000);

  Serial.print("cm: ");
  Serial.println(echo_time[0] / 58); //divide time by 58 to get

  return ping_time;
}

int backPing() {
  //Back ultrasonic
  digitalWrite(ULTRASONIC_IN_PIN_BACK, HIGH);
  delayMicroseconds(10); //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ULTRASONIC_IN_PIN_BACK, LOW);

  // Use command pulseIn to listen to ultrasonic_Data pin to record the
  // time that it takes from when the Pin goes HIGH until it goes LOW

  unsigned long ping_time = pulseIn(ULTRASONIC_OUT_PIN_BACK, HIGH, 10000);


  Serial.print("cm: ");
  Serial.println(ping_time / 58); //divide time by 58 to get distance in cm
  return ping_time;

}


void getEncoderPos()
{
                        Serial.print("Rot: ");
			Serial.println(encoder_TopMotor.getRawPosition());
			Serial.print("Encoders L: ");
			Serial.print(encoder_LeftMotor.getRawPosition());
			Serial.print(", R: ");
			Serial.print(encoder_RightMotor.getRawPosition());
			

}

boolean hitTable() {
  int bottom_lever = digitalRead(FRONT_BOTTOM_LEVER_SWITCH_PIN);
  int top_lever = digitalRead(FRONT_TOP_LEVER_SWITCH_PIN);
  if ((bottom_lever == LOW) && (top_lever == HIGH)){
    Serial.println("Table");
    return true;
  }
  else{
    Serial.println("Nothing");
    return false;
  }
}

boolean hitWall() {
  int bottom_lever = digitalRead(FRONT_BOTTOM_LEVER_SWITCH_PIN);
  int top_lever = digitalRead(FRONT_TOP_LEVER_SWITCH_PIN);
  if ((top_lever == LOW) && (bottom_lever == LOW)){
    Serial.println("Wall");
    return true;

  }
  else{
    Serial.println("Nothing");
    return false;
  }

}


// -------------------- MOVEMENT FUNCTIONS --------------------


// Smart movement functions
void allOfTin(){
  currentReading = frontPing(); // update reading
  integral += currentReading; // add reading to integral

}

// Forward and reverse movement functions
void turnLeftAngle(long angle)
{
    calcLeftTurn(2000, angle);     
    
    while (!doneLeftTurn())
    {
      turnLeftOnSpot(200); 
    } 
    setNeutral();
}

void turnRightAngle(long angle)
{
    calcRightTurn(2000, angle);     
    
    while (!doneRightTurn())
    {
      turnRightOnSpot(200); 
    } 
    setNeutral();
}
void moveForwardFixed(){
  Left_Motor_Speed = 1700;
  Right_Motor_Speed = 1700;
  implementMotorSpeed();
}

void moveBackwardsFixed(){
  Left_Motor_Speed = 1300;
  Right_Motor_Speed = 1300;
  implementMotorSpeed();
}


void moveForward(long speedFactor)
{
  Left_Motor_Speed = constrain((Left_Motor_Stop + speedFactor), 1500, 2100);
  Right_Motor_Speed = constrain((Right_Motor_Stop + speedFactor), 1500, 2100);
  implementMotorSpeed();
}

void moveBackwards(long speedFactor)
{
  Left_Motor_Speed = constrain((Left_Motor_Stop - speedFactor), 900, 1500);
  Right_Motor_Speed = constrain((Right_Motor_Stop - speedFactor), 900, 1500);
  implementMotorSpeed();
}
void moveBackDistance(long distance)
{
  leftEncoderStopTime = encoder_LeftMotor.getRawPosition();
  while (leftEncoderStopTime + distance > encoder_LeftMotor.getRawPosition())
  {
    Left_Motor_Speed = constrain((Left_Motor_Stop - 300), 1500, 2100);
    Right_Motor_Speed = constrain((Right_Motor_Stop - 300), 1500, 2100);
    implementMotorSpeed();
  }

}

void backUp() {
  setNeutral();
  delay(1000);
  moveBackwardsFixed();
  delay(1500);
  setNeutral();
}

// Turn functions

void veerRight(long speedFactor, long intensity)
{
  Left_Motor_Speed = constrain((Left_Motor_Stop + speedFactor + intensity), 1500, 2100);
  Right_Motor_Speed = constrain((Right_Motor_Stop + speedFactor), 1500, 2100);
  implementMotorSpeed();
}

void veerLeft(long speedFactor, long intensity)
{
  Left_Motor_Speed = constrain((Left_Motor_Stop + speedFactor), 1500, 2100);
  Right_Motor_Speed = constrain((Right_Motor_Stop + speedFactor + intensity), 1500, 2100);
  implementMotorSpeed();
}

void turnLeftOnSpot(long speedFactor)
{
  Left_Motor_Speed = constrain((Left_Motor_Stop - speedFactor), 900, 1500);
  Right_Motor_Speed = constrain((Right_Motor_Stop + speedFactor), 1500, 2100);
  implementMotorSpeed();
}
void turnRightOnSpot(long speedFactor)
{
  Left_Motor_Speed = constrain((Left_Motor_Stop + speedFactor), 1500, 2100);
  Right_Motor_Speed = constrain((Right_Motor_Stop - speedFactor), 900, 1500);
  implementMotorSpeed();
}
void calcLeftTurn(long fullCircle, int angle) // full circle should be around 2800
{
  rightEncoderStopTime = encoder_RightMotor.getRawPosition();
  rightEncoderStopTime += (fullCircle * angle) / 360;
}
void calcRightTurn(long fullCircle, int angle)
{
  leftEncoderStopTime = encoder_LeftMotor.getRawPosition();
  leftEncoderStopTime += (fullCircle * angle) / 360;
}

boolean doneLeftTurn()
{
  if (encoder_RightMotor.getRawPosition() > rightEncoderStopTime)
    return true;
  else
    return false;
}
boolean doneRightTurn()
{
  if (encoder_LeftMotor.getRawPosition() > leftEncoderStopTime)
    return true;
  else
    return false;
}

void turnClockwise(long speedFactor) {
  Left_Motor_Speed = constrain((Left_Motor_Stop + speedFactor), 1500, 2100);
  Right_Motor_Speed = Left_Motor_Stop;
  implementMotorSpeed();
}

void pivotLeft() {

}

void pivotRight() {
}

// Implementation movement functions

void implementMotorSpeed()
{
  servo_LeftMotor.writeMicroseconds(constrain((Left_Motor_Speed + Left_Motor_Offset), 900, 2100));
  servo_RightMotor.writeMicroseconds(constrain((Right_Motor_Speed + Right_Motor_Offset), 900, 2100));
}

void setNeutral() {
  Left_Motor_Speed = 1500;
  Right_Motor_Speed = 1500;
  implementMotorSpeed();

}

void brake() {
  Left_Motor_Speed = 200;
  Right_Motor_Speed = 200;
  implementMotorSpeed();
}


// -------------------- TIME FUNCTIONS --------------------


// Call startWaiting first, and then waitMilliSecond
void startWaiting() {
  if (can_start_waiting) {

    time_previous = millis();
  }
}

boolean waitMilliSecond(unsigned int interval) {

  time_elapsed = millis();
  if ((time_elapsed - time_previous) > interval) {
    can_start_waiting = false;
    return true; // Done waiting!
  }
  else {
    return false; // Not done waiting!
  }
}

void allOfTin()
{
  // find perpendicualr distance to closer ultrasonic sensor with delay to prevent interference
  float frontReading = (float)frontPing();
  delay(3);
  float backReading = (float)backPing();
  delay(3);

  // update smaller perpendicular distance (ultrasonic that is closer)
  if (frontReading < backReading)
    currentReading = (frontReading * dSensors)/(sqrt((float)dSensors * dSensors + (frontReading - backReading) * (frontReading - backReading)));
  else
    currentReading = (backReading * dSensors)/(sqrt((float)dSensors * dSensors + (frontReading - backReading) * (frontReading - backReading)));

  // update angle
  
}


