#include "WiFiConnection.h"
#include <DMALedMatrix.h>

DMALedMatrix led;

WiFiConnection::WiFiConnection(const char *ssid, const char *password, wifi_mode_t mode)
{
    this->ssid = ssid;
    this->password = password;
    this->mode = mode;

    this->connect();
}

WiFiConnection::~WiFiConnection()
{
    // this->disconnect();
}

void WiFiConnection::connect()
{
    WiFi.mode(this->mode);
    WiFi.begin(this->ssid, this->password);
    Serial.print("Connecting to WiFi");
    Serial.println(ssid);
    led.clearScreen();
    led.drawTextWrap(0, 0, "Connecting to WiFi", "#0000ff", 1);
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        led.drawTextWrap(counter, 16, ". ", "#0000ff", 1);
        counter++;
        if (counter > 20)
        {
            Serial.println();
            Serial.println("Can't connect to WiFi!");
            led.clearScreen();
            led.drawTextWrap(0, 0, "Failed to connect WiFi!", "#ff0000", 1);
            delay(2000);
            break;
        }
    }
    Serial.print("Connected to WiFi: ");
    Serial.println(WiFi.localIP());
}

void WiFiConnection::disconnect()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("Disconnected from WiFi");
}

bool WiFiConnection::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void WiFiConnection ::accessPoint(const char *ssid, const char *password)
{
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0), IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, password);
    Serial.println("Access point started");
    led.clearScreen();
    led.drawTextWrap(0, 0, "Access point started", "#0000ff", 1);
    delay(2000);
    led.clearScreen();
    led.drawTextWrap(0, 0, "IP Address: ", "#0000ff", 1);
    led.drawTextWrap(0, 16, WiFi.softAPIP().toString().c_str(), "#0000ff", 1);
    delay(5000);
}
