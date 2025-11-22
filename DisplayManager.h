#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "BasicManager.h"
#include "MotorManager.h"
#include "ServoManager.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class DisplayManager : public BasicManager {
  public:
    enum BOOTSTAT {
      BOOT_START = 0,
      WIFI_CONNECTING, 
      WIFI_CONNECTED, 
      WIFI_GOT_IP, 
      WEBSERVER_START, 
      WEBSERVER_READY
    };

    DisplayManager();

    void init() override;
    void update(unsigned long now) override;

    void setStat(BOOTSTAT stat);
    void setIPAddress(const String& ip);
    void setInfo(uint8_t max_output, MotorManager::Direction dir, ServoManager::Angle angle);
    BOOTSTAT getStat() const;

  private:
    Adafruit_SSD1306 display;
    BOOTSTAT stat = BOOT_START;

    String ipAddress = "";
    uint8_t motor_max_output = 0;
    MotorManager::Direction dir = MotorManager::FORWARD;
    ServoManager::Angle angle = ServoManager::STR;

    void show();
};

#endif