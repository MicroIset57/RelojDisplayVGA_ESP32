#include "sys/time.h"
#include "sys/_timeval.h"
#include "lwip/apps/sntp.h"
#include "esp_sntp.h"
#include <Arduino.h>

typedef void (*StatusFunc)(const char *texto);

class HoraNtp
{
   private:
    const char *HORA_FECHA = "%H:%M:%S %d/%m/%Y";  // "13:24:35 14/12/22"
    const char *HORA       = "%H:%M:%S";           // "13:24:35"
    const char *FECHA      = "%d/%m/%Y";           // "14/12/22"

    HoraNtp() {}  // Constructor privado para evitar la creación de instancias.

   public:
    // Método estático para obtener la instancia única de la clase.
    static HoraNtp &Instance()
    {
        static HoraNtp instance;  // Instancia única en tiempo de ejecución.
        return instance;
    }

    bool EstoyEnHora()
    {
        return time(0) > 1694133206;  // Friday, 8 September 2023 0:33:26
    }

    // Obtiene un string con la hora:
    String Format(const char *format)
    {
        struct tm ttm;
        time_t T;
        time(&T);
        localtime_r((time_t *)&T, &ttm);
        static char buf[20];
        strftime(buf, 20, format, &ttm);
        return String(buf);
    }

    String HoraFecha() { return Format(HORA_FECHA); }

    String Fecha() { return Format(FECHA); }

    String Hora() { return Format(HORA); }

    // espera forever que se ponga en hora con un servidor NTP.
    void Scan()
    {
        // NTP Servers para probar:
        // time.google.com
        // 2.ar.pool.ntp.org
        // 3.south-america.pool.ntp.org
        // 1.south-america.pool.ntp.org

        sntp_setoperatingmode(SNTP_OPMODE_POLL);                // connect to NTP server
        sntp_setservername(0, "time.google.com");               //
        sntp_setservername(1, "2.ar.pool.ntp.org");             //
        sntp_setservername(2, "3.south-america.pool.ntp.org");  //
        sntp_setservername(3, "1.south-america.pool.ntp.org");  //
        sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);               //
        sntp_init();                                            // comienza el scanning para poner en hora

        Serial.print("Esperando la hora");

        bool hora_real_seteada = false;
        int milicentonios      = 0;
        do
        {
            ++milicentonios;
            delay(666);  // si consulto muy rapido , el server me banea!
            hora_real_seteada = (sntp_get_sync_status() != SNTP_SYNC_STATUS_RESET);

            Serial.print('.');

            if (milicentonios % 12 == 0) Serial.println("Hey!");
            if (milicentonios > 55)
            {
                Serial.println("\nYa esperé 55 milicentonios, ABORT!");
                break;
            }
        } while (!hora_real_seteada);
        sntp_stop();

        // sale del loop si se pone en hora!!

        if (hora_real_seteada)
        {
            // esto setea para Arg:
            time_t T = time(0);
            setenv("TZ", "GMT+3", 1);  // Argentina
            tzset();
            struct timeval tv = {(time_t)T, (suseconds_t)0};
            settimeofday(&tv, NULL);

            Serial.print("\nYA ESTOY EN HORA => ");
            Serial.println(HoraFecha());
        }
    }
};