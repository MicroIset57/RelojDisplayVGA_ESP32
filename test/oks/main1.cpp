/*
  ISET 57
  Septiembre, 2023
*/

#include "NTP.h"
#include <SimpleDHT.h>
#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "ISET57CLARO";
const char *password = "GONZALO1981";

int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

void setup()
{
  Serial.begin(115200);
  delay(100);

  // Inicializa la conexión WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conexión exitosa a WiFi");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());

  HoraNtp::Instance().Scan(); // pone en hora
}

// mainloop
void loop()
{
  Serial.println("=================================");
  Serial.println("Sample DHT11...");

  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT11 failed, err=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    delay(1000);
    return;
  }

  Serial.print("Sample OK: ");
  Serial.print((int)temperature);
  Serial.print(" *C, ");
  Serial.print((int)humidity);
  Serial.println(" H");

  // DHT11 sampling rate is 1HZ.
  delay(1500);
}