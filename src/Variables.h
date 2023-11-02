#pragma once
#include <Arduino.h>

#define Amarillo "\033[33m"
#define Rojo     "\033[31m"
#define Verde    "\033[32m"
#define NoColor  "\033[37m"

// DHT11 ultima medida
extern byte temperature;
extern byte humidity;

// los 3 valores ultimos leidos del archivo de forecast en 3 horarios fijos 9-12-19
extern float temperature_2m[3];             // %
extern float precipitation_probability[3];  // %
extern float cloudcover[3];                 // %

// valores maximos por 4 días a partir de mañana
extern float temperature_2m_max[4];             // °C
extern float temperature_2m_min[4];             // °C
extern float precipitation_sum[4];              // h
extern float precipitation_probability_max[4];  // %
