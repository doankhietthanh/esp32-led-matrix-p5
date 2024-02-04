#include "RestAPI.h"

RestAPI::RestAPI()
{
}

RestAPI::~RestAPI()
{
}

String RestAPI::get(const char *endpoint)
{
    String response = "{}";
    if (http.begin(client, endpoint))
    {
        int httpCode = http.GET();
        if (httpCode > 0)
        {
            response = http.getString();
        }
        else
        {
            Serial.print("[ERROR] GET: ");
            Serial.print(endpoint);
            Serial.println(http.errorToString(httpCode).c_str());
        }
        http.end();
    }
    return response;
}