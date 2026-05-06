#include <Arduino.h>
#include <PS4Controller.h>
#include "utilities/constants.h"

variables constants;
// PS4 controller instance

// Limit switch state
volatile bool stopMotorLeft = false;
volatile bool stopMotorRight = false;

unsigned long lastStepMicros = 0;
bool stepState = LOW;

void IRAM_ATTR stopLeft();
void IRAM_ATTR stopRight();
void updateStepper(bool moveLeft, bool moveRight);
void stopStepper();

void setup() {
  pinMode(constants.ledSol, OUTPUT);
  pinMode(constants.enable, OUTPUT);
  pinMode(constants.direccion, OUTPUT);
  pinMode(constants.stepper, OUTPUT);

  pinMode(constants.limitLeft, INPUT_PULLUP);
  pinMode(constants.limitRight, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(constants.limitLeft), stopLeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(constants.limitRight), stopRight, CHANGE);

  Serial.begin(115200);

  PS4.begin(constants.ps4Mac);
  Serial.println("Waiting for PS4 controller...");
}

void loop() {
  if (!PS4.isConnected()) {
    digitalWrite(constants.ledSol, LOW);
    stopStepper();
    return;
  }

  const bool estadoSol = PS4.Cross();
  digitalWrite(constants.ledSol, estadoSol);

  const int joystick = PS4.LStickX();
  const bool moveRight = joystick > constants.deadzone;
  const bool moveLeft = joystick < -constants.deadzone;

  if ((moveRight && stopMotorRight) || (moveLeft && stopMotorLeft)) {
    stopStepper();
  } else {
    updateStepper(moveLeft, moveRight);
  }

  Serial.print("Controller LX: ");
  Serial.println(joystick);

  Serial.print("Left switch: ");
  Serial.println(digitalRead(constants.limitLeft));

  Serial.print("Right switch: ");
  Serial.println(digitalRead(constants.limitRight));

  Serial.print("Direction: ");
  Serial.println(digitalRead(constants.direccion));

  delay(20);
}

void IRAM_ATTR stopLeft() {
  stopMotorLeft = digitalRead(constants.limitLeft) == LOW;
}

void IRAM_ATTR stopRight() {
  stopMotorRight = digitalRead(constants.limitRight) == LOW;
}

void updateStepper(bool moveLeft, bool moveRight) {
  if (!moveLeft && !moveRight) {
    stopStepper();
    return;
  }

  digitalWrite(constants.enable, HIGH);
  digitalWrite(constants.direccion, moveRight ? HIGH : LOW);

  const unsigned long now = micros();

  if (now - lastStepMicros >= constants.stepInterval) {
    lastStepMicros = now;
    stepState = !stepState;
    digitalWrite(constants.stepper, stepState);
  }
}

void stopStepper() {
  digitalWrite(constants.enable, LOW);
  digitalWrite(constants.stepper, LOW);
  stepState = LOW;
}