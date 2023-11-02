#include "Variables.h"

byte temperature = 0;  // DHT11 ultima medida
byte humidity    = 0;  // DHT11

// los 3 valores ultimos leidos del archivo de forecast en 3 horarios fijos 9-12-19
float temperature_2m[3];             // %
float precipitation_probability[3];  // %
float cloudcover[3];                 // %

// valores maximos por 4 días a partir de mañana
float temperature_2m_max[4];             // °C
float temperature_2m_min[4];             // °C
float precipitation_sum[4];              // h
float precipitation_probability_max[4];  // %
