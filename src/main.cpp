#include <main.h>

DMALedMatrix dmMatrix;
WiFiUDP ntpUdp;
NTPClient timeClient(ntpUdp, NTP_SERVER, GMT_OFFSET_SEC);
AsyncWebServer webServer(80);
RestAPI restAPI;
FileSytem fileSytem;

JsonDocument settingsDoc;
bool wifiState = WiFiState::WIFI_DISCONNECTED;
String gifDir = "/gifs";
char Time[] = "00:00";
char Date[] = "00/00/2000";
char Temperature[] = "00oC";
char Humidity[] = "00%";
const char *WeatherIconCode;
char *weatherIcon = icon_bits[0];

void setup()
{
  // Setup Serial
  Serial.begin(SERIAL_BAUDRATE);
  // Setup SPIFFS
  SPIFFS.begin();
  // Hello World
  dmMatrix.clearScreen();
  dmMatrix.drawGif("/store/loading.io-64x32px.gif");
  // Setup Wifi
  String settingsDocStr = fileSytem.readFile(SETTINGS_FILE);
  JsonDocument wifiDoc;
  deserializeJson(settingsDoc, settingsDocStr);
  wifiDoc = settingsDoc["wifi"];
  wifiSSID = wifiDoc["ssid"];
  wifiPassword = wifiDoc["pass"];
  WiFiConnection wifi(wifiSSID, wifiPassword, WIFI_MODE);
  wifiState = wifi.isConnected() ? WiFiState::WIFI_CONNECTED : WiFiState::WIFI_DISCONNECTED;
  if (wifiState == WiFiState::WIFI_DISCONNECTED)
  {
    wifi.accessPoint(WIFI_AP_SSID, WIFI_AP_PASSWORD);
    settings();
  }
  // Setup NTP Client
  timeClient.begin();
  // GET Weather
  getWeather();
  // Clear LED Screen
  dmMatrix.clearScreen();
}

unsigned long clock_get_last_time = 0;
unsigned long weather_get_last_time = 0;
unsigned long screen_get_last_time = 0;
unsigned int screen = Screen::WEATHER;
uint8_t wheelval = 0;

void loop()
{
  if ((millis() - screen_get_last_time) > NEXT_SCREEN_TIMEOUT)
  {
    screen++;
    screen_get_last_time = millis();
    dmMatrix.clearScreen();

    if (screen_get_last_time > 60000 && wifiState == WiFiState::WIFI_DISCONNECTED)
    {
      ESP.restart();
    }
  }

  switch (screen)
  {
  case Screen::CLOCK:
    displayClock();
    break;
  case Screen::PICTURE:
    displayPicture();
    break;
  case Screen::WEATHER:
    displayWeather();
    break;
  case Screen::GIFS:
    displayGifs();
    break;
  case Screen::RESET:
  default:
    screen = 0;
    break;
  };
}

void setTimeout(unsigned long timeout)
{
  if ((millis() - screen_get_last_time) > timeout)
  {
    screen++;
    screen_get_last_time = millis();
  }
}

void displayClock()
{
  if (wifiState == WiFiState::WIFI_DISCONNECTED)
  {
    screen++;
    return;
  }
  timeClient.update();
  int second_, minute_, hour_, day_, month_, year_;
  unsigned long epochTime = timeClient.getEpochTime();
  second_ = second(epochTime);
  if (clock_get_last_time != second_)
  {
    minute_ = minute(epochTime);
    hour_ = hour(epochTime);
    day_ = day(epochTime);
    month_ = month(epochTime);
    year_ = year(epochTime);

    Time[4] = minute_ % 10 + 48;
    Time[3] = minute_ / 10 + 48;
    Time[2] = second_ % 2 == 0 ? ':' : ' ';
    Time[1] = hour_ % 10 + 48;
    Time[0] = hour_ / 10 + 48;

    Date[0] = day_ / 10 + 48;
    Date[1] = day_ % 10 + 48;
    Date[3] = month_ / 10 + 48;
    Date[4] = month_ % 10 + 48;
    Date[8] = (year_ / 10) % 10 + 48;
    Date[9] = year_ % 10 % 10 + 48;

    dmMatrix.clearScreen();
    dmMatrix.drawColorWheelText(3, 2, Time, 2, (int)epochTime);
    dmMatrix.drawColorWheelText(3, 2 + 20, Date, 1, (int)epochTime);
    clock_get_last_time = second_;

    delay(500);
  }
}

void displayWeather()
{
  if (wifiState == WiFiState::WIFI_DISCONNECTED)
  {
    screen++;
    return;
  }
  weatherIcon = mapIconCode(WeatherIconCode);
  dmMatrix.drawXbm565(0, 0, 32, 32, weatherIcon);
  dmMatrix.drawText(36, 4, Temperature, RED_COLOR, 1);
  dmMatrix.drawText(36, 20, Humidity, BLUE_COLOR, 1);
  if ((millis() - weather_get_last_time) > GET_WEATHER_API_TIMEOUT)
  {
    getWeather();
    weather_get_last_time = millis();
  }
}

void displayGifs()
{
  dmMatrix.drawGifs(gifDir);
}

void displayPicture()
{
  int count = 0;
  while (count < 5)
  {
    dmMatrix.drawGif("/picture/phaohoa_1.gif");
    count++;
  }
  count = 0;
  while (count < 5)
  {
    dmMatrix.drawGif("/picture/phaohoa_2.gif");
    count++;
  }
  count = 0;
  while (count < 5)
  {
    dmMatrix.drawGif("/picture/phaohoa_3.gif");
    count++;
  }
}

void getWeather()
{
  if (settingsDoc.isNull())
  {
    String settingsDocStr = fileSytem.readFile(SETTINGS_FILE);
    deserializeJson(settingsDoc, settingsDocStr);
  }
  JsonDocument locationDoc;
  locationDoc = settingsDoc["location"];
  latitude = locationDoc["lat"];
  longtitude = locationDoc["lon"];
  Serial.println("Latitude: ");
  Serial.println(latitude);
  Serial.println("Longtitude: ");
  Serial.println(longtitude);
  String serverPath = "http://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude) + "&lon=" + String(longtitude) + "&appid=" + String(OPEN_WEATHER_API_KEY);
  String response = restAPI.get(serverPath.c_str());

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, response);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  serializeJsonPretty(doc, Serial);

  WeatherIconCode = doc["weather"][0]["icon"];

  float temperature = doc["main"]["temp"];
  float humidity = doc["main"]["humidity"];

  Temperature[0] = (int)(temperature - 273) / 10 + 48;
  Temperature[1] = (int)(temperature - 273) % 10 + 48;

  Humidity[0] = (int)humidity / 10 + 48;
  Humidity[1] = (int)humidity % 10 + 48;
}

char *mapIconCode(const char *code)
{
  for (int i = 0; i < sizeof(icon_codes) / sizeof(icon_codes[0]); ++i)
  {
    if (strcmp(icon_codes[i], code) == 0)
    {
      return icon_bits[i];
    }
  }
  return weatherIcon;
}

void settings()
{
  // data = {wifi: {ssid: "ssid", pass: "pass"}}
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/html", index_html); });
  webServer.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
               {
      JsonDocument wifiDoc;
      JsonDocument locationDoc;
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST wifi ssid value
          if (p->name() == "ssid") {
            String ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            wifiDoc["ssid"] = ssid;
          }
          // HTTP POST wifi pass value
          if (p->name() == "pass") {
            String pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            wifiDoc["pass"] = pass;
          }
          // HTTP POST location lat value
          if (p->name() == "lat") {
            String lat = p->value().c_str();
            Serial.print("Latitude set to: ");
            Serial.println(lat);
            locationDoc["lat"] = lat;
          }
          // HTTP POST location lon value
          if (p->name() == "lon") {
            String lon = p->value().c_str();
            Serial.print("Longitude set to: ");
            Serial.println(lon);
            locationDoc["lon"] = lon;
          }
        }
      }
      settingsDoc["wifi"] = wifiDoc;
      settingsDoc["location"] = locationDoc;
      String docStr;
      serializeJson(settingsDoc, docStr);
      Serial.println("Data: ");
      Serial.println(docStr);
      fileSytem.writeFile(SETTINGS_FILE, docStr.c_str());
      request->send(200, "text/plain", "Done. ESP will restart, please waiting esp reconnect to new wifi.");
      delay(3000);
      ESP.restart(); });
  webServer.begin();
}
