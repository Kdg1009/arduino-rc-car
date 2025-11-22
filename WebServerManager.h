#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <WiFiS3.h>

class WebServerManager {
public:
    WebServerManager();

    void init();
    void update(unsigned long now);

    bool isRunning() const;

    // ----- API 등록용 -----
    void attachMotorOutputCallback(void (*cb)(uint8_t));
    void attachMotorDirCallback(void (*cb)(int));
    void attachServoAngleCallback(void (*cb)(int));

private:
    WiFiServer server;
    bool _running = false;

    // callback functions
    void (*motorOutputCallback)(uint8_t) = nullptr;
    void (*motorDirCallback)(int) = nullptr;
    void (*servoAngleCallback)(int) = nullptr;

    // API handlers
    void handleClient();
    void sendResponse(WiFiClient& client, int code, const char* contentType, const char* content);
    void sendHTMLResponse(WiFiClient& client);
    String urlDecode(String str);
    String getParam(String data, String param);
};

#endif
