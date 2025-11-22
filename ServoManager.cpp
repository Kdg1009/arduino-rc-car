#include "ServoManager.h"

void ServoManager::init() {
  servo.attach(PWM);
  angle = STR;
  servo.write(angle);

  initialized = true;
}

void ServoManager::update(unsigned long now) {
  if (!initialized) return;
  applyServoOutput();
}

void ServoManager::setAngle(ServoManager::Angle new_angle) {
  angle = new_angle;
}

ServoManager::Angle ServoManager::getAngle() const {
  return angle;
}

void ServoManager::applyServoOutput() {
  servo.write(static_cast<int>(angle));
}