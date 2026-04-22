#include <Arduino.h>
#include <Stepper.h>
#include <wire.h>

// Define motor steps per revolution (28BYJ-48 has 2048 steps/rev with gearbox)
const int STEPS_PER_REV = 2048;

// Pin connections to ULN2003 driver IN1-IN4
const int IN1 = 8;
const int IN2 = 9;
const int IN3 = 10;
const int IN4 = 11;

// Create Stepper object
Stepper stepperMotor(STEPS_PER_REV, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(9600);
  Serial.println("Stepper Motor Control with ULN2003 Driver");

  // Set motor speed (RPM)
  stepperMotor.setSpeed(5); // Adjust for smoothness and torque
}

void loop() {
  // Example: rotate one revolution clockwise
  Serial.println("Rotating CW...");
  stepperMotor.step(STEPS_PER_REV);
}
