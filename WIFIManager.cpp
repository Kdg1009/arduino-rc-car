#include "WIFIManager.h"

void WiFiManager::init(const char* ssid, const char* pass) {
  _ssid = ssid;
  _pass = pass;
}

bool WiFiManager::connect() {
  WiFi.begin(_ssid, _pass);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 5000) {
    delay(200);
  }

  _connected = (WiFi.status() == WL_CONNECTED);
  return _connected;
}

void WiFiManager::update(unsigned long now) {
  if (!_connected) {
    // if not connected to wifi, retry every 5 seconds
    if (now - _lastRetry >= RETRY_INTERVAL) {
      connect();
      _lastRetry = now;
    }
  } else {
    _connected = (WiFi.status() == WL_CONNECTED);
  }
}

bool WiFiManager::isConnected() const {
  return _connected;
}

String WiFiManager::getIPAddress() const {
  if (!_connected) return "";
  return WiFi.localIP().toString();
}