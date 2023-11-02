# ESP32-Display_Reloj_Temp_VGA

## Proyecto_Display_Temp

Lista de deseos: 📃

- Hacer Circuito y plaqueta con ESP32 + resistencias + conector VGA + Sensores de Temperatura/Humedad - Listo ✔️
- Buscar libreria para servidor NTP - Listo ✔️
- Diseñar Marco en 3D (En partes para poder imprimir en el instituto) para el display LCD con ventana para visualizar - Micro ESP32
- Buscar libreria para exponer informacion de clima ☀️ Listo ✔️
- calendario de feriados 🗓️
- Mostrar reloj 🕙
- (open-meteo.com - para sacar informacion del clima y elegir que datos traer)
- Mostrar Calendario academico.

### Ejemplos de servicios

- https://www.meteomatics.com/en/api/getting-started/
- https://weather.com/es-AR/tiempo/hoy/l/ARBA0009:1:AR?Goto=Redirected
- https://openweathermap.org/api
- https://open-meteo.com/en/docs#hourly=temperature_2m,windspeed_10m
    - ESTE VAMOS A USAR 😃

---

### Ejemplo con OPEN METEO

http://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,precipitation_probability,cloudcover&daily=temperature_2m_max,temperature_2m_min,precipitation_sum,precipitation_probability_max&timeformat=unixtime&timezone=auto&forecast_days=5&format=csv

Primer listado que contiene el csv:

- time,
- temperature_2m (°C),
- precipitation_probability (%),
- cloudcover (%)

Segundo listado del csv:

- time,
- temperature_2m_max (°C),
- temperature_2m_min (°C),
- precipitation_sum (mm),
- precipitation_probability_max (%)
 

# Mostrar info de un mensaje con TELEGRAM

# Mostrar feriados proximos

