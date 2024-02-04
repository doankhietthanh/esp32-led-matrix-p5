#ifndef __REST_API_H
#define __REST_API_H

#include <WiFi.h>
#include <HTTPClient.h>

class RestAPI
{
private:
    WiFiClient client;
    HTTPClient http;

public:
    RestAPI();
    ~RestAPI();
    String get(const char *endpoint);
};

#endif