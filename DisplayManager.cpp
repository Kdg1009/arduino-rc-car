#include "DisplayManager.h"

DisplayManager::DisplayManager() : display(128, 32, &Wire, -1) {}

void DisplayManager::init() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  stat = DisplayManager::BOOT_START;
  show();

  initialized = true;
}

void DisplayManager::update(unsigned long now) {
  if (!initialized) return;
  show();
}

void DisplayManager::setStat(DisplayManager::BOOTSTAT new_stat) {
  stat = new_stat;
}

void DisplayManager::setIPAddress(const String& ip) {
  ipAddress = ip;
}

void DisplayManager::setInfo(uint8_t max_output, MotorManager::Direction new_dir, ServoManager::Angle new_angle) {
  motor_max_output = max_output;
  dir = new_dir;
  angle = new_angle;
}

DisplayManager::BOOTSTAT DisplayManager::getStat() const {
  return stat;
}

void DisplayManager::show() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  switch (stat) {
    case DisplayManager::BOOT_START:
      display.println("BOOT START...");
      break;

    case DisplayManager::WIFI_CONNECTING:
      display.println("WIFI CONNECTING...");
      break;

    case DisplayManager::WIFI_CONNECTED:
      display.println("WIFI CONNECTED");
      break;

    case DisplayManager::WIFI_GOT_IP:
      display.println("GOT IP:");
      display.println(ipAddress);
      break;

    case DisplayManager::WEBSERVER_START:
      display.println("WEBSERVER START...");
      break;

    case DisplayManager::WEBSERVER_READY:
      display.print("IP: ");
      display.println(ipAddress);

      display.print("SPD: ");
      display.print(motor_max_output);
      display.print(" DIR: ");
      switch (dir) {
        case MotorManager::FORWARD:
          display.println("FWD");
          break;
        case MotorManager::BACKWARD:
          display.println("BACK");
          break;
        case MotorManager::STOP:
          display.println("STOP");
          break;
      }

      display.print("STEER: ");
      if (angle == ServoManager::STR) display.println("STR");
      else if (angle == ServoManager::LEFT) display.println("LEFT");
      else if (angle == ServoManager::RIGHT) display.println("RIGHT");
      break;
  }

  display.display();
}