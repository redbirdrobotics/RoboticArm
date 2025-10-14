#include <Servo.h>

// Define motor pins
#define BASE_PIN 7
#define BASE_PRIME_PIN 6
#define ELBOW_PIN 8
#define END_PIVOT_PIN 9
#define END_ROTATE_PIN 10
#define CLAW_PIN 11

// Current servo angles
float bAngle = 90;
float bPrimeAngle = 90;
float elbowAngle = 90;
float ePivotAngle = 90;
float eRotateAngle = 90;
float clawAngle = 45;

// Velocity variables (degrees per update)
float bVel = 0;
float bPrimeVel = 0;
float elbowVel = 0;
float ePivotVel = 0;
float eRotateVel = 0;
float clawVel = 0;

// Max speeds (deg/update)
const float BASE_MAX = 2;
const float ELBOW_MAX = 3;
const float PIVOT_MAX = 3;
const float ROTATE_MAX = 5;
const float CLAW_MAX = 4;

// Accelerations (deg/update^2)
const float BASE_ACC = 0.1;
const float ELBOW_ACC = 0.15;
const float PIVOT_ACC = 0.15;
const float ROTATE_ACC = 0.25;
const float CLAW_ACC = 1;

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

  base.write(bAngle);
  basePrime.write(bPrimeAngle);
  elbow.write(elbowAngle);
  endPivot.write(ePivotAngle);
  endRotate.write(eRotateAngle);
  claw.write(clawAngle);
}

float updateServo(float angle, float &vel, bool positive, bool negative,
                  float a_max, float v_max, int minA, int maxA) {
  // acceleration or deceleration
  if (positive && !negative)
    vel += a_max;
  else if (negative && !positive)
    vel -= a_max;
  else {
    // natural slowdown (friction)
    if (vel > a_max) vel -= a_max;
    else if (vel < -a_max) vel += a_max;
    else vel = 0;
  }

  // clamp velocity
  if (vel > v_max) vel = v_max;
  if (vel < -v_max) vel = -v_max;

  // integrate
  angle += vel;
  return constrain(angle, minA, maxA);
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    switch (c) {
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

  // Update with acceleration curves
  bAngle = updateServo(bAngle, bVel, wPressed, sPressed, BASE_ACC, BASE_MAX, 0, 180);
  bPrimeAngle = 180 - bAngle; // keep mirrored

  elbowAngle = updateServo(elbowAngle, elbowVel, aPressed, dPressed, ELBOW_ACC, ELBOW_MAX, 0, 180);
  eRotateAngle = updateServo(eRotateAngle, eRotateVel, ePressed, qPressed, ROTATE_ACC, ROTATE_MAX, 0, 180);
  ePivotAngle = updateServo(ePivotAngle, ePivotVel, rPressed, fPressed, PIVOT_ACC, PIVOT_MAX, 30, 150);
  clawAngle = updateServo(clawAngle, clawVel, xPressed, zPressed, CLAW_ACC, CLAW_MAX, 0, 72);

  // Write positions
  base.write((int)bAngle);
  basePrime.write((int)bPrimeAngle);
  elbow.write((int)elbowAngle);
  endPivot.write((int)ePivotAngle);
  endRotate.write((int)eRotateAngle);
  claw.write((int)clawAngle);

  delay(20);
}
