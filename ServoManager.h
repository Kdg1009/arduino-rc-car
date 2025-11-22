#ifndef SERVO_MANAGER_H
#define SERVO_MANAGER_H

#include "BasicManager.h"
#include <Servo.h>

class ServoManager : public BasicManager {
  public:
    enum Angle {STR = 105, LEFT = 90, RIGHT = 120};

    void init() override;
    void update(unsigned long now) override;

    void setAngle(Angle angle);

    Angle getAngle() const;

  private:
    const uint8_t PWM = 6;

    Angle angle = STR;
    Servo servo;

    void applyServoOutput();
};

#endif