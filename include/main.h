#include <HardwareSerial.h>

#include <WiFiConnection.h>
#include <DMALedMatrix.h>
#include <RestAPI.h>
#include <FileSystem.h>

#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "weather_icons.h"

#define SERIAL_BAUDRATE 9600
#define SETTINGS_FILE "/settings.txt"

#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 25200
#define DAYLIGHT_OFFSET_SEC 3600

#define WIFI_MODE WIFI_MODE_STA
#define WIFI_AP_SSID "ESP32 Settings"
#define WIFI_AP_PASSWORD "12345678"
const char *wifiSSID;
const char *wifiPassword;

#define GET_WEATHER_API_TIMEOUT 60000 * 60 // 15 minutes
#define NEXT_SCREEN_TIMEOUT 15000          // 15 seconds

#define OPEN_WEATHER_API_KEY "74c4cf8d380edd68852bbdb668130937"
const char *latitude = "10.762622";
const char *longtitude = "106.660172";

enum WiFiState
{
    WIFI_DISCONNECTED,
    WIFI_CONNECTED
};

enum Screen
{
    CLOCK = 0,
    WEATHER,
    GIFS,
    RESET,
};

const char *RED_COLOR = "#FF0000";
const char *BLUE_COLOR = "#0000FF";

const char *icon_codes[18] = {
    "01d", "02d", "03d", "04d", "09d", "10d", "11d", "13d", "50d",
    "01n", "02n", "03n", "04n", "09n", "10n", "11n", "13n", "50n"};

static char *icon_bits[18] = {
    sun_bits,
    cloud_sun_bits,
    clouds_bits,
    cloud_wind_sun_bits,
    rain0_bits,
    rain1_sun_bits,
    rain_lightning_bits,
    snow_bits,
    wind_bits,
    moon_bits,
    cloud_moon_bits,
    cloud_bits,
    cloud_wind_moon_bits,
    rain0_bits,
    rain1_moon_bits,
    rain_lightning_bits,
    snow_bits,
    wind_bits};

void settings();
void displayClock();
void displayWeather();
void displayGifs();
void setTimeout(unsigned long timeout);
void getWeather();
char *mapIconCode(const char *code);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <title>ESP Wi-Fi Manager</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style type="text/css">
        html {
            font-family: Arial, Helvetica, sans-serif;
            display: inline-block;
            text-align: center;
        }

        h1 {
            font-size: 1.8rem;
            color: white;
        }

        p {
            font-size: 1.4rem;
        }

        .topnav {
            overflow: hidden;
            background-color: #0a1128;
        }

        body {
            margin: 0;
        }

        .content {
            padding: 5%;
        }

        .card-grid {
            max-width: 800px;
            margin: 0 auto;
            display: grid;
            grid-gap: 2rem;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
        }

        .card {
            background-color: white;
            box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, 0.5);
        }

        .card-title {
            font-size: 1.2rem;
            font-weight: bold;
            color: #034078;
        }

        input[type="submit"] {
            border: none;
            color: #fefcfb;
            background-color: #034078;
            padding: 15px 15px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            width: 100px;
            margin-right: 10px;
            border-radius: 4px;
            transition-duration: 0.4s;
        }

        input[type="submit"]:hover {
            background-color: #1282a2;
        }

        input[type="text"],
        input[type="number"],
        select {
            width: 50%;
            padding: 12px 20px;
            margin: 18px;
            display: inline-block;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }

        label {
            font-size: 1.2rem;
        }

        .value {
            font-size: 1.2rem;
            color: #1282a2;
        }

        .state {
            font-size: 1.2rem;
            color: #1282a2;
        }

        button {
            border: none;
            color: #fefcfb;
            padding: 15px 32px;
            text-align: center;
            font-size: 16px;
            width: 100px;
            border-radius: 4px;
            transition-duration: 0.4s;
        }

        .button-on {
            background-color: #034078;
        }

        .button-on:hover {
            background-color: #1282a2;
        }

        .button-off {
            background-color: #858585;
        }

        .button-off:hover {
            background-color: #252524;
        }
    </style>
</head>

<body>
    <div class="topnav">
        <h1>ESP Settings</h1>
    </div>
    <div class="content">
        <div class="card-grid">
            <div class="card">
                <form action="/" method="POST">
                    <p>
                        <label for="ssid">WiFi SSID</label>
                        <input type="text" id="ssid" name="ssid" value="P5"><br>

                        <label for="pass">WiFi Password</label>
                        <input type="text" id="pass" name="pass" value="anhthien85"><br>

                        <label for="lat">Latitude</label>
                        <input type="text" id="lat" name="lat" value="10.762622"><br>

                        <label for="lon">Longtitude</label>
                        <input type="text" id="lon" name="lon" value="106.660172"><br>
                        
                        <input type="submit" value="Submit">
                    </p>
                </form>
            </div>
        </div>
    </div>
</body>

</html>
)rawliteral";