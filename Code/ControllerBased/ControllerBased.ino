#include <Servo.h>        // Include the Servo library
#include <AccelStepper.h> // Include the AccelStepper library for Stepper object control

// Define input pins
#define rBumper 3
#define lBumper 2
#define rJoySwitch 4
#define lJoySwitch 5

#define lJoyx A0
#define lJoyy A1
#define rJoyx A2
#define rJoyy A3

// Define motor pins
#define BASE_PIN 7
#define BASE_PRIME_PIN 6
#define ELBOW_PIN 8
#define END_PIVOT_PIN 9
#define END_ROTATE_PIN 10
#define CLAW_PIN 11

// #define DIR_PIN 13
// #define STEP_PIN 12

// Define pins for stepper motor microstepping control
// MS1 is connected to A4 and MS2 to A5.
// #define MS1_PIN A4
// #define MS2_PIN A5

// Joystick neutral positions
const int joystickThreshold = 25; // Threshold for joystick movement detection
int LXNeutral = 595; 
int LYNeutral = 633; 
int RXNeutral = 623; 
int RYNeutral = 609; 

int bAngle = 90;
int bPrimeAngle = 90;
int elbowAngle = 90;    // Elbow servo angle
int ePivotAngle = 90;   // End Pivot servo angle
int eRotateAngle = 90;  // End Rotate servo angle
int clawAngle = 45;     // Claw servo angle (101/2 = 50.5, rounded to 51)

// Declare Servo objects for each joint
Servo base;
Servo basePrime;
Servo elbow;
Servo endPivot;
Servo endRotate;
Servo claw;

// Declare AccelStepper object for the stepper motor
// AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup() {
  // Attach servo objects to their respective pins.
  base.attach(BASE_PIN);
  basePrime.attach(BASE_PRIME_PIN);
  elbow.attach(ELBOW_PIN);
  endPivot.attach(END_PIVOT_PIN);
  endRotate.attach(END_ROTATE_PIN);
  claw.attach(CLAW_PIN);

  // Configure input pins as pullup for reduced noise
  // INPUT_PULLUP means the pin will be HIGH by default, and LOW when button/switch is pressed
  pinMode(lBumper, INPUT_PULLUP);
  pinMode(rBumper, INPUT_PULLUP);
  pinMode(lJoySwitch, INPUT_PULLUP);
  pinMode(rJoySwitch, INPUT_PULLUP);

  //calibrate controller neutrals
  const int numSamples = 100;
  long lxSum = 0, lySum = 0, rxSum = 0, rySum = 0;

  for (int i = 0; i < numSamples; i++) {
    lxSum += analogRead(lJoyx);
    lySum += analogRead(lJoyy);
    rxSum += analogRead(rJoyx);
    rySum += analogRead(rJoyy);
    delay(10); // Small delay between readings
  }

  LXNeutral = lxSum / numSamples;
  LYNeutral = lySum / numSamples;
  RXNeutral = rxSum / numSamples;
  RYNeutral = rySum / numSamples;


  // Begin serial communication for debugging and displaying input
  Serial.begin(115200);
  claw.write(45);
  endRotate.write(90);
  endPivot.write(90);
  elbow.write(90);
  base.write(90);
  basePrime.write(90);

}

void loop() {
  // Read joystick analog values
  int leftX = analogRead(lJoyx);
  int leftY = analogRead(lJoyy);
  int rightX = analogRead(rJoyx);
  int rightY = analogRead(rJoyy);

  // Read bumper and joystick switch digital states
  // Use !digitalRead because INPUT_PULLUP means LOW when pressed, HIGH when released
  bool rBumperState = !digitalRead(rBumper);
  bool lBumperState = !digitalRead(lBumper);
  bool rJoyState = !digitalRead(rJoySwitch);
  bool lJoyState = !digitalRead(lJoySwitch);

  // --- Servo Motor Control based on Joystick/Bumper Input ---


// // Stepper Motor Control (left joystick X axis) using continuous speed
// if (abs(leftX - LXNeutral) > joystickThreshold) {
//   int speed = map(leftX, 0, 1023, -2000, 2000);
//   stepper.setSpeed(speed);
//   stepper.runSpeed();  // Move at constant speed
// } else {
// stepper.setSpeed(0); // Stop movement
// }



  // Baseplate servos (controlled by leftY joystick)
  if (abs(leftY - LYNeutral) > joystickThreshold) {
    // Map joystick input to a delta value for angle change
    int delta = map(leftY, 0, 1023, -3, 3); // Adjust delta range for gentler movement
    bAngle = constrain(bAngle + delta, 0, 180);       // Constrain base angle between 0 and 180
    bPrimeAngle = constrain(bPrimeAngle - delta, 0, 180); // Constrain base prime angle between 0 and 180
    base.write(bAngle);
    basePrime.write(bPrimeAngle);
  }

  // End Rotator (controlled by rightX joystick)
  if (abs(rightX - RXNeutral) > joystickThreshold) {
    int delta = map(rightX, 0, 1023, -6, 6); // Adjusted delta range for gentler movement
    eRotateAngle = constrain(eRotateAngle + delta, 0, 180); // Constrain end rotate angle between 0 and 180
    endRotate.write(eRotateAngle);
  }

  // End Pivot (controlled by rightY joystick)
  if (abs(rightY - RYNeutral) > joystickThreshold) {
    int delta = map(rightY, 0, 1023, 5, -5); // Adjusted delta range for gentler movement
    ePivotAngle = constrain(ePivotAngle + delta, 30, 150); // Constrain end pivot angle between 0 and 180
    endPivot.write(ePivotAngle);
  }

  // ELBOW (controlled by leftX joystick)
  if (abs(leftX - LXNeutral) > joystickThreshold) {
    int delta = map(leftX, 0, 1023, -3, 3); // Adjusted delta range for gentler movement
    elbowAngle = constrain(elbowAngle + delta, 0, 180); // Constrain end rotate angle between 0 and 180
    elbow.write(elbowAngle);
  }

  // Claw (controlled by joystick switches)
  if (lJoyState || rJoyState) {
    int switchState = (lJoyState << 1) | rJoyState; // Create a 2-bit state (LJS RJS)
    switch(switchState) {
      case 0b00: // Both off or both pressed (no change)
      case 0b11:
        break;
      case 0b10: // Left joystick switch pressed (decrement claw angle)
        clawAngle = constrain(clawAngle - 3, 0, 100); // Constrain claw angle between 0 and 90
        claw.write(clawAngle);
        break;
      case 0b01: // Right joystick switch pressed (increment claw angle)
        clawAngle = constrain(clawAngle + 3, 0, 100); // Constrain claw angle between 0 and 90
        claw.write(clawAngle);
        break;
    };
  }

  // Small delay to allow servos to reach position and prevent excessive loop speed
  delay(20); // Reduced delay for more responsive servo control

  // --- Debug: Display all controller input and stepper position ---
  Serial.print("Left X: ");
  Serial.print(leftX);
  Serial.print(" | Left Y: ");
  Serial.print(leftY);
  Serial.print(" | Right X: ");
  Serial.print(rightX);
  Serial.print(" | Right Y: ");
  Serial.print(rightY);
  Serial.print(" | LJS: ");
  Serial.print(lJoyState);
  Serial.print(" | RJS: ");
  Serial.print(rJoyState);
  Serial.print(" | LBumper: ");
  Serial.print(lBumperState);
  Serial.print(" | RBumper: ");
  Serial.print(rBumperState);
  // Serial.print(" | Stepper Pos: ");
  // Serial.print(stepper.currentPosition());
  Serial.print(" | Base: ");
  Serial.print(bAngle);
  Serial.print(" | Base Prime: ");
  Serial.print(bPrimeAngle);
  Serial.print(" | Elbow: ");
  Serial.print(elbowAngle);
  Serial.print(" | End Pivot: ");
  Serial.print(ePivotAngle);
  Serial.print(" | End Rotate: ");
  Serial.print(eRotateAngle);
  Serial.print(" | Claw: ");
  Serial.println(clawAngle);
}
