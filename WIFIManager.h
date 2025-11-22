#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFiS3.h>

class WiFiManager {
  public:
    void init(const char* ssid, const char* pass);
    bool connect();
    void update(unsigned long now);

    bool isConnected() const;
    String getIPAddress() const;

  private:
    const char* _ssid;
    const char* _pass;

    bool _connected = false;
    unsigned long _lastRetry = 0;
    const unsigned long RETRY_INTERVAL = 5000; // retry every 5 seconds
};

#endif