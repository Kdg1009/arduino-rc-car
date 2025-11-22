#include "MotorManager.h"

void MotorManager::init() {
  max_output = 200;
  direction = STOP;

  // pinMode setting
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // ensure MotorManager is initialized
  initialized = true;
}

void MotorManager::update(unsigned long now) {
  if (!initialized) return;
  applyMotorOutput();
}

void MotorManager::setMaxOutput(uint8_t rate) {
  // analogWrite can accept 0 ~ 255  integer
  max_output = constrain(rate, 0, 255);
}

void MotorManager::setDirection(MotorManager::Direction dir) {
  direction = dir;
}

uint8_t MotorManager::getMaxOutput() const {
  return max_output;
}

MotorManager::Direction MotorManager::getDirection() const {
  return direction;
}

void MotorManager::applyMotorOutput() {
  uint8_t speed = max_output;
  Serial.print("<MotorManager log> speed: ");
  Serial.print(speed);
  Serial.print(" direction: ");
  Serial.println(direction);

  setMotor(IN1, IN2, ENA, speed);
  setMotor(IN3, IN4, ENB, speed);
}

void MotorManager::setMotor(uint8_t in1, uint8_t in2, uint8_t en, uint8_t speed) {
  switch (direction) {
    case STOP:
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      break;
    case FORWARD:
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      break;
    case BACKWARD:
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      break;
  }
  analogWrite(en, speed);
}