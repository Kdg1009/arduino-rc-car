#ifndef MOTOR_MANAGER_H
#define MOTOR_MANAGER_H

#include "BasicManager.h"
#include <Arduino.h>

class MotorManager : public BasicManager {
  public:
    enum Direction {FORWARD = 0, BACKWARD, STOP};

    void init() override;
    void update(unsigned long now) override;
    
    void setMaxOutput(uint8_t rate);
    void setDirection(Direction dir);
    
    uint8_t getMaxOutput() const;
    Direction getDirection() const;

  private:
    const uint8_t IN1 = 2;
    const uint8_t IN2 = 3;
    const uint8_t ENA = 5;
    
    const uint8_t IN3 = 7;
    const uint8_t IN4 = 8;
    const uint8_t ENB = 9;

    uint8_t max_output = 0;
    Direction direction = STOP;

    void applyMotorOutput();
    void setMotor(uint8_t in1, uint8_t in2, uint8_t en, uint8_t speed);
};

#endif