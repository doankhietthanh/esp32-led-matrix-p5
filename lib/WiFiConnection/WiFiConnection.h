#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WString.h>
#include <HardwareSerial.h>

class WiFiConnection
{
private:
    const char *ssid;
    const char *password;
    wifi_mode_t mode;

public:
    WiFiConnection(const char *ssid, const char *password, wifi_mode_t mode);
    ~WiFiConnection();
    void connect();
    void disconnect();
    bool isConnected();
    void accessPoint(const char *ssid, const char *password);
};

#endif