#include <WiFi.h>
#include <HTTPClient.h>
#include <EEPROM.h>
#include "Forecast.h"
#include "Variables.h"

// Archivos a bajar y luego analizar:
#define TEMPE_CSV         "/tempe.csv"
#define PRECIPITACION_CSV "/precip.csv"
#define COVER_CSV         "/cover.csv"
#define DAILY_CSV         "/daily.csv"

// devuelve segundos de mañana a la hora señalada:
time_t GetTomorrowTime(int hour)
{
    time_t segsTomorrow = time(0) + 3600 * 24;
    struct tm *tm       = localtime(&segsTomorrow);
    tm->tm_hour         = hour;
    tm->tm_min          = 0;
    tm->tm_sec          = 0;
    // Serial.printf(">>>  %lu   %d   %d \n", mktime(tm), tm->tm_hour, tm->tm_mday);
    return mktime(tm);  // armo segs con fecha de mañana a las x hs
}

#define ESTADO_INICIAL        0
#define ESTADO_PRIMER_BLOQUE  1
#define ESTADO_SEGUNDO_BLOQUE 2
#define ESTADO_FINAL          3

// como los 3 archivos son iguales, le paso el puntero al array donde tengo que guardar los datos.
void ActualizarVariable(String NombreDeArchivo, float *ArrayForecast)
{
    int estado           = ESTADO_INICIAL;  // tengo que leer 2 bloques diferentes en el archivo
    int indiceDeHora     = -1;              // voy a procesar las horas: 9, 12 y 19
    int horasBuscadas[3] = {9, 12, 19};     // "
    time_t segundos      = 0;

    File f = SPIFFS.open(NombreDeArchivo, "r");
    if (!f)
    {
        Serial.println(Rojo "error al abrir csv" NoColor);
        return;
    }

    while (f.available())
    {
        String linea = f.readStringUntil('\n');
        if (linea[0] != '1') continue;  // salteo todo hasta la fecha que comienza con 1.

        char *tiempo = strtok((char *)linea.c_str(), ",");
        char *val    = strtok(0, ",\n");
        if (!tiempo || !val) continue;  // algun error...

        time_t LineTime = atoi(tiempo);

        if (LineTime > segundos && indiceDeHora < 3)
        {
            if (indiceDeHora >= 0)
            {
                ArrayForecast[indiceDeHora] = atof(val);
                Serial.printf("%dhs:\t [%d]  %.2f\n", horasBuscadas[indiceDeHora], LineTime, ArrayForecast[indiceDeHora]);
            }

            // busco la siguiente hora
            indiceDeHora++;
            if (indiceDeHora <= 2) segundos = GetTomorrowTime(horasBuscadas[indiceDeHora]);
        }
    }
    f.close();
}

void ActualizarVariablesDaily(String NombreDeArchivo)
{
    int indiceDeHora = 0;
    bool salteado    = false;

    File f = SPIFFS.open(NombreDeArchivo, "r");
    if (!f)
    {
        Serial.println(Rojo "error al abrir csv" NoColor);
        return;
    }

    while (f.available())
    {
        String linea = f.readStringUntil('\n');
        if (linea[0] != '1') continue;  // salteo todo hasta la fecha que comienza con 1.

        if (!salteado)
        {
            salteado = true;
            continue;
        }

        //  time,temperature_2m_max (°C),temperature_2m_min (°C),precipitation_sum (mm),precipitation_probability_max (%)
        char *tiempo      = strtok((char *)linea.c_str(), ",");  // vienen 5 fechas (nada mas) que son los 5 dias
        char *tempe_max   = strtok(0, ",");                      // °C
        char *tempe_min   = strtok(0, ",");                      // °C
        char *presipi_sum = strtok(0, ",");                      // mm
        char *presipi_max = strtok(0, ",\n");                    // %
        if (!tiempo || !tempe_max || !tempe_min || !presipi_sum || !presipi_max) continue;

        temperature_2m_max[indiceDeHora]            = atof(tempe_max);
        temperature_2m_min[indiceDeHora]            = atof(tempe_min);
        precipitation_sum[indiceDeHora]             = atof(presipi_sum);
        precipitation_probability_max[indiceDeHora] = atof(presipi_max);

        Serial.printf("> %.2f   %.2f    %.2f    %.2f\n", temperature_2m_max[indiceDeHora], temperature_2m_min[indiceDeHora], precipitation_sum[indiceDeHora], precipitation_probability_max[indiceDeHora]);
        indiceDeHora++;  // prox valor
        if (indiceDeHora == 4) break;
    }
    f.close();
}

void ActualizarVariablesForecast()
{
    ActualizarVariable(TEMPE_CSV, temperature_2m);
    ActualizarVariable(PRECIPITACION_CSV, precipitation_probability);
    ActualizarVariable(COVER_CSV, cloudcover);
    ActualizarVariablesDaily(DAILY_CSV);
}

void Download(String filename, String url)
{
    if (WiFi.status() != WL_CONNECTED) return;  // todavia no conectado

    Serial.print("Download ");
    Serial.print(filename);
    Serial.print(" ");
    Serial.println(url);

    // Petición HTTP GET al servicio de forecast:
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);

    bool ok                 = true;
    long TiempoTranscurrido = millis();
    int code                = http.GET();
    if (code == 200)
    {
        Serial.print(" wait...");
        File archivo = SPIFFS.open(filename, "w");
        if (archivo)
        {
            http.writeToStream(&archivo);
            archivo.close();
            Serial.print(Verde "archivo guardado");
            ok = true;
        }
    }
    else
    {
        Serial.print(Rojo "HTTP ERROR ");
        Serial.print(code);
        ok = false;
    }
    http.end();  // aca se dispara el error  flush(): fail on fd 48, errno: 11, "No more processes"
    // pero se graba igual el archivo :)

    Serial.print(" en ");
    Serial.print(millis() - TiempoTranscurrido);
    Serial.println("ms" NoColor);
}

void DownloadForecast()
{
    time_t segs  = time(0) + 3600 * 24;  // mañana
    struct tm *t = localtime(&segs);
    char date[12];
    strftime(date, 12, "%Y-%m-%d", t);  // formato 2023-10-07

    String s1 = "http://api.open-meteo.com/v1/forecast?latitude=-32.9520382&longitude=-60.7790416&hourly=";
    String s2 = "&timeformat=unixtime&timezone=America%2FSao_Paulo&start_date=" + String(date) + "&end_date=" + String(date) + "&format=csv";
    Download(String(TEMPE_CSV), s1 + "temperature_2m" + s2);
    Download(String(PRECIPITACION_CSV), s1 + "precipitation_probability" + s2);
    Download(String(COVER_CSV), s1 + "cloudcover" + s2);

    String url
        = "http://api.open-meteo.com/v1/"
          "forecast?latitude=-32.9468&longitude=-60.6393&daily=temperature_2m_max,temperature_2m_min,precipitation_sum,precipitation_probability_max&timeformat=unixtime&timezone=America%2FSao_Paulo&forecast_days=5&format=csv";
    // bajo 1 solo horario con todo junto:
    Download(String(DAILY_CSV), url);
}
