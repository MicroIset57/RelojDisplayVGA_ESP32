/*
  ISET 57
  -RELOJ USANDO MONITOR VGA
  -PIDE DATOS DE TEMPERATURA Y HUMEDAD
  -PIDE A UN WERV WEB DATOS DE PRONOSTICO DE TIEMPO

  Septiembre, 2023

   cc by-sa 4.0 license
   bitluni
*/

#include <SimpleDHT.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <TaskScheduler.h>
#include "NTP.h"
#include "Variables.h"
#include "ShowVga.h"
#include "Forecast.h"

/*
You can use this token to access HTTP API:
5888907281:AAEu6CvKZoPJib_eS86vN7oyUIRwc8_hi0U
For a description of the Bot API, see this page: https://core.telegram.org/bots/api
*/

// minutos para bajar el csv
#define PERIODO_DOWNLOAD_FORECAST    (60 * TASK_MINUTE)
#define PERIODO_LEER_DHT11           (15 * TASK_SECOND)
#define PERIODO_ACTUALIZAR_VARIABLES (10 * TASK_MINUTE)
// Parametros de conexion WIFI
#define WIFI_SSID "ISET57CLARO"
#define WIFI_PASS "GONZALO1981"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "5888907281:AAEu6CvKZoPJib_eS86vN7oyUIRwc8_hi0U"
char TextoTelegram[250];

const unsigned long BOT_MTBS = 1000;  // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;  // last time messages' scan has been done
bool Start = false;

SimpleDHT11 dht11(23);
void VerificarDHT11();

void handleNewMessages(int numNewMessages)
{
    for (int i = 0; i < numNewMessages; i++)
    {
        String chat_id = bot.messages[i].chat_id;
        String text    = bot.messages[i].text;

        strcpy(TextoTelegram, text.c_str());

        String from_name = bot.messages[i].from_name;
        if (from_name == "") from_name = "Guest";

        if (text == "/start")
        {
            String welcome = "Hola " + from_name + ".\n";
            welcome += "Podes enviar texto a la pantalla.\n\n";
            bot.sendMessage(chat_id, welcome);
        }
        Serial.print(text);
        Serial.print(" ");
    }
}

//----------------------------------------------------------------------------
// Schedulers
Task TaskDownloadForecast(PERIODO_DOWNLOAD_FORECAST, TASK_FOREVER, &DownloadForecast);
Task TaskVerificarDHT11(PERIODO_LEER_DHT11, TASK_FOREVER, &VerificarDHT11);
Task TaskActualizarForecast(PERIODO_ACTUALIZAR_VARIABLES, TASK_FOREVER, &ActualizarVariablesForecast);
Scheduler scheduler;
//----------------------------------------------------------------------------

void ConectarWifi()
{
    Serial.print("Conectando a WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }

    Serial.print("\nWiFi OK, IP:");
    Serial.println(WiFi.localIP());

    // si todavia no esta en hora... ponete en hora
    if (!HoraNtp::Instance().EstoyEnHora())
    {
        HoraNtp::Instance().Scan();

        // se baja en el reset, y luego cada varias horas.
        static bool PrimeraVez = true;
        if (PrimeraVez)
        {
            PrimeraVez = false;
            DownloadForecast();

            TaskDownloadForecast.enable();
            TaskVerificarDHT11.enable();
            TaskActualizarForecast.enable();
        }
    }
}

// Leo el DHT11 cada 4 segundos.
// se actualizan las variables temperature, humidity
void VerificarDHT11()
{
    if (dht11.read(&temperature, &humidity, NULL) != SimpleDHTErrSuccess)
    {
        Serial.println(Rojo "Error al leer DHT11" NoColor);
    }
    else
    {
        Serial.print("Temp:");
        Serial.print((int)temperature);
        Serial.print(" Hum:");
        Serial.println((int)humidity);
    }
}

void setup()
{
    Serial.begin(115200);
    SPIFFS.begin(true, "/spiffs", 5);
    delay(1000);
    Serial.println("inicializando...");

    // Inicializa la conexión WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org

    // arranca el temporizador
    scheduler.init();
    // scheduler.addTask(TaskDownloadForecast);
    scheduler.addTask(TaskVerificarDHT11);
    scheduler.addTask(TaskActualizarForecast);
    // todos se encienden recien cuando está en hora...

    // esta tarea es solo para mostrar los frames en el VGA:
    VgaStart();
}

void loop()
{
    scheduler.execute();  // run del temporizador.

    if (WiFi.status() != WL_CONNECTED) ConectarWifi();  // conecta y muestra la hora en el status.

    ActualizarPantallas();

    if (millis() - bot_lasttime > BOT_MTBS)
    {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages)
        {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }

        bot_lasttime = millis();
    }
}
