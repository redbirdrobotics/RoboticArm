#include <Servo.h>

// Define motor pins
#define BASE_PIN 7
#define BASE_PRIME_PIN 6
#define ELBOW_PIN 8
#define END_PIVOT_PIN 9
#define END_ROTATE_PIN 10
#define CLAW_PIN 11

// Current servo angles
int bAngle = 90;
int bPrimeAngle = 90;
int elbowAngle = 90;
int ePivotAngle = 90;
int eRotateAngle = 90;
int clawAngle = 45;

// Step speeds in degrees per update
const int BASE_SPEED = 1;
const int ELBOW_SPEED = 2;
const int PIVOT_SPEED = 2;
const int ROTATE_SPEED = 4;
const int CLAW_SPEED = 3;

// Press flags
bool wPressed = false;
bool sPressed = false;
bool aPressed = false;
bool dPressed = false;
bool qPressed = false;
bool ePressed = false;
bool rPressed = false;
bool fPressed = false;
bool zPressed = false;
bool xPressed = false;

// Servo objects
Servo base;
Servo basePrime;
Servo elbow;
Servo endPivot;
Servo endRotate;
Servo claw;

void setup() {
  Serial.begin(115200);

  base.attach(BASE_PIN);
  basePrime.attach(BASE_PRIME_PIN);
  elbow.attach(ELBOW_PIN);
  endPivot.attach(END_PIVOT_PIN);
  endRotate.attach(END_ROTATE_PIN);
  claw.attach(CLAW_PIN);

  // Initialize positions
  base.write(bAngle);
  basePrime.write(bPrimeAngle);
  elbow.write(elbowAngle);
  endPivot.write(ePivotAngle);
  endRotate.write(eRotateAngle);
  claw.write(clawAngle);
}

void loop() {
  // handle serial input
  while (Serial.available() > 0) {
    char c = Serial.read();
    switch (c) {
      // press events
      case 'w': wPressed = true; break;
      case 's': sPressed = true; break;
      case 'a': aPressed = true; break;
      case 'd': dPressed = true; break;
      case 'q': qPressed = true; break;
      case 'e': ePressed = true; break;
      case 'r': rPressed = true; break;
      case 'f': fPressed = true; break;
      case 'z': zPressed = true; break;
      case 'x': xPressed = true; break;

      // release events (uppercase from Python)
      case 'W': wPressed = false; break;
      case 'S': sPressed = false; break;
      case 'A': aPressed = false; break;
      case 'D': dPressed = false; break;
      case 'Q': qPressed = false; break;
      case 'E': ePressed = false; break;
      case 'R': rPressed = false; break;
      case 'F': fPressed = false; break;
      case 'Z': zPressed = false; break;
      case 'X': xPressed = false; break;
    }
  }

  // update angles based on flags
  if (wPressed) {
    bAngle = constrain(bAngle + BASE_SPEED, 0, 180);
    bPrimeAngle = constrain(bPrimeAngle - BASE_SPEED, 0, 180);
  }
  if (sPressed) {
    bAngle = constrain(bAngle - BASE_SPEED, 0, 180);
    bPrimeAngle = constrain(bPrimeAngle + BASE_SPEED, 0, 180);
  }
  if (aPressed) {
    elbowAngle = constrain(elbowAngle + ELBOW_SPEED, 0, 180);
  }
  if (dPressed) {
    elbowAngle = constrain(elbowAngle - ELBOW_SPEED, 0, 180);
  }
  if (qPressed) {
    eRotateAngle = constrain(eRotateAngle - ROTATE_SPEED, 0, 180);
  }
  if (ePressed) {
    eRotateAngle = constrain(eRotateAngle + ROTATE_SPEED, 0, 180);
  }
  if (rPressed) {
    ePivotAngle = constrain(ePivotAngle + PIVOT_SPEED, 30, 150);
  }
  if (fPressed) {
    ePivotAngle = constrain(ePivotAngle - PIVOT_SPEED, 30, 150);
  }
  if (zPressed) {
    clawAngle = constrain(clawAngle - CLAW_SPEED, 0, 72);
  }
  if (xPressed) {
    clawAngle = constrain(clawAngle + CLAW_SPEED, 0, 72);
  }

  //write positions to servos
  base.write(bAngle);
  basePrime.write(bPrimeAngle);
  elbow.write(elbowAngle);
  endPivot.write(ePivotAngle);
  endRotate.write(eRotateAngle);
  claw.write(clawAngle);

  // optional debug output
  /*
  Serial.print("Base: "); Serial.print(bAngle);
  Serial.print(" | Base Prime: "); Serial.print(bPrimeAngle);
  Serial.print(" | Elbow: "); Serial.print(elbowAngle);
  Serial.print(" | End Pivot: "); Serial.print(ePivotAngle);
  Serial.print(" | End Rotate: "); Serial.print(eRotateAngle);
  Serial.print(" | Claw: "); Serial.println(clawAngle);
  */

  delay(20);
}
