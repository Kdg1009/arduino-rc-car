#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <Arduino.h>
#include "BasicManager.h"
#include "WiFiManager.h"
#include "WebServerManager.h"
#include "DisplayManager.h"
#include "MotorManager.h"
#include "ServoManager.h"

enum BootStep {
  BOOT_START = 0,
  BOOT_WIFI_CONNECTING,
  BOOT_WIFI_CONNECTED,
  BOOT_WIFI_GOT_IP,
  BOOT_WEBSERVER_START,
  BOOT_READY
};

class StateManager {
  public:
    static StateManager& instance();

    void init(const char* ssid, const char* pass);
    void update(unsigned long now);

    BootStep getBootStep() const;
    String getIPAddress() const;

    void cmd_setMotorSpeed(uint8_t rate);
    void cmd_setMotorDir(int dir);
    void cmd_setSteering(int angle);

  private:
    StateManager();

    WiFiManager wifi;
    WebServerManager server;
    DisplayManager display;
    MotorManager motor;
    ServoManager servo;

    BootStep bootStep = BOOT_START;
    unsigned long lastUpdateMs = 0;

    void setBootStep(BootStep s);
};

#endif
