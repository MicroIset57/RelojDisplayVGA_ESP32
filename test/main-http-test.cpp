#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SPIFFS.h>
#include <FS.h>

const char *ForecastURL
    = "http://api.open-meteo.com/v1/"
      "forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,precipitation_probability,cloudcover&daily=temperature_2m_max,temperature_2m_min,precipitation_sum,precipitation_probability_max&timeformat=unixtime&timezone=auto&forecast_days=5&format=csv";
void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin("ISET57CLARO", "GONZALO1981");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);
    }
    Serial.println(" wifi ok");

    SPIFFS.begin(true);

    // Transfer-Encoding: chunked
    HTTPClient http;
    http.setTimeout(5000);
    http.begin(ForecastURL);
    // http.addHeader("Transfer-Encoding", "chunked");

    Serial.println("HTTPs Connection Successfull");

    File file = SPIFFS.open("/1", "w");
    if (file)
    {
        long T               = millis();
        int httpResponseCode = http.GET();
        if (httpResponseCode == 200)
        {
            Serial.print("downloading...");
            http.writeToStream(&file);
            Serial.println("Archivo guardado con éxito.");
        }
        else
        {
            Serial.print("HTTP ERROR ");
            Serial.println(httpResponseCode);
        }
        file.close();
        Serial.printf("------------------- %d ms\n", millis() - T);
    }
    http.end();

    file = SPIFFS.open("/1", "r");
    while (file.available())
    {
        Serial.println(file.readStringUntil('\n'));
    }
    file.close();
    Serial.println("-------------------");
}

void loop() {}