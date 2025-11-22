#include "StateManager.h"

StateManager& StateManager::instance() {
  static StateManager inst;
  return inst;
}

StateManager::StateManager() {}

void StateManager::init(const char* ssid, const char* pass) {
  // 1. init display
  display.init();
  setBootStep(BOOT_START);
  Serial.println("<State Manager log> BOOT START");

  // 2. connect to wifi
  setBootStep(BOOT_WIFI_CONNECTING);
  wifi.init(ssid, pass);
  if (wifi.connect()) {
    setBootStep(BOOT_WIFI_CONNECTED);
    display.setIPAddress(wifi.getIPAddress());
    setBootStep(BOOT_WIFI_GOT_IP);
    Serial.println("<State Manager log> WiFi connected");
  } else {
    Serial.println("<State Manager log> WiFi not connected");
  }
  
  // 3. motor/servo init
  motor.init();
  servo.init();
  Serial.println("<State Manager log> motors init");
  // 4. webserver init
  setBootStep(BOOT_WEBSERVER_START);
  Serial.println("<State Manager log> Launching webserver...");
  server.attachMotorOutputCallback([](uint8_t value) {
    StateManager::instance().cmd_setMotorSpeed(value);
  });
  
  server.attachMotorDirCallback([](int dir) {
    StateManager::instance().cmd_setMotorDir(dir);
  });

  server.attachServoAngleCallback([](int angle) {
    StateManager::instance().cmd_setSteering(angle);
  });

  server.init(); // register routes but not start blocking
  setBootStep(BOOT_READY);
  Serial.println("<State Manager log> Ready to go!");
  // display show ready state and init info
  display.setStat(DisplayManager::WEBSERVER_READY);
  display.setInfo(motor.getMaxOutput(), motor.getDirection(), servo.getAngle());

  lastUpdateMs = millis();
}

void StateManager::update(unsigned long now) {
  wifi.update(now);
  server.update(now);
  motor.update(now);
  servo.update(now);
  display.update(now);

  display.setInfo(motor.getMaxOutput(), motor.getDirection(), servo.getAngle());
  
  static bool prevWifiConnected = false;
  bool connected = wifi.isConnected();
  
  if (connected && !prevWifiConnected) {
    // newly connected
    display.setIPAddress(wifi.getIPAddress());
    display.setStat(DisplayManager::WIFI_GOT_IP);
  } else if (!connected && prevWifiConnected) {
    // lost connection
    display.setStat(DisplayManager::WIFI_CONNECTING);
  } else {
    display.setStat(DisplayManager::WEBSERVER_READY);
  }
  prevWifiConnected = connected;

  lastUpdateMs = now;
}

BootStep StateManager::getBootStep() const { return bootStep; }
String StateManager::getIPAddress() const { return wifi.getIPAddress(); }

// command from webserver
void StateManager::cmd_setMotorSpeed(uint8_t rate) {
  motor.setMaxOutput(rate);
}

void StateManager::cmd_setMotorDir(int dir) {
  if (dir == 0) {
    motor.setDirection(MotorManager::FORWARD);
  } else if (dir == 1) {
    motor.setDirection(MotorManager::BACKWARD);
  } else if (dir == 2) {
    motor.setDirection(MotorManager::STOP);
  }
}

void StateManager::cmd_setSteering(int angle) {
  if (angle <= ServoManager::LEFT) {
    servo.setAngle(ServoManager::LEFT);
  } else if (ServoManager::RIGHT <= angle) {
    servo.setAngle(ServoManager::RIGHT);
  } else {
    servo.setAngle(ServoManager::STR);
  }
}

void StateManager::setBootStep(BootStep s) {
  bootStep = s;

  switch (s) {
    case BOOT_START:
      display.setStat(DisplayManager::BOOT_START);
      break;

    case BOOT_WIFI_CONNECTING:
      display.setStat(DisplayManager::WIFI_CONNECTING);
      break;

    case BOOT_WIFI_CONNECTED:
      display.setStat(DisplayManager::WIFI_CONNECTED);
      break;

    case BOOT_WIFI_GOT_IP:
      display.setStat(DisplayManager::WIFI_GOT_IP);
      break;

    case BOOT_WEBSERVER_START:
      display.setStat(DisplayManager::WEBSERVER_START);
      break;

    case BOOT_READY:
      display.setStat(DisplayManager::WEBSERVER_READY);
      break;
  }
  display.update(0);
}