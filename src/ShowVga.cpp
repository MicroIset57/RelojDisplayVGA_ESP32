
#include "Variables.h"
#include "ShowVga.h"
#include "MiVga6.h"
// Libs de Bitluni (VGA)
#include <ESP32Lib.h>

extern char *TextoTelegram;

// VGA Device using an interrupt to unpack the pixels from 4bit to 16bit for the I²S
// This takes some CPU time in the background but is able to fit a frame buffer in the memory
VGA6Bit vga;
const PinConfig &pinConfig = VGA6Bit::PicoVGA;

int ancho, alto;  // de la pantalla

// barra de estado inferior
void PrintStatus(const char *texto)
{
    vga.setFont(Font8x8);
    vga.setCursor(5, alto - 8);
    vga.setTextColor(vga.RGB(128, 128, 128));
    vga.print(texto);
}

void VgaStart()
{
    // enabling double buffering
    vga.setFrameBufferCount(2);
    // Mode MiMonitor(8, 54, 28, 360, 11, 2, 32, 480, 2, 14161000, 1, 0);
    // MiMonitor.print<HardwareSerial>(Serial);
    // vga.init(MiMonitor, pinConfig);
    vga.init(vga.MODE320x240, pinConfig);  // ok, muy bien
    // vga.init(vga.MODE360x200, pinConfig);
    ancho = vga.mode.hRes - 1;  // tamaño de pantalla
    alto  = vga.mode.vRes / 2 - 1;
    // Serial.print("alto");
    // Serial.println(alto);
    vga.clear();  // limpio pantalla

    // esta tarea es solo para mostrar los frames en el VGA:
    // TaskHandle_t xHandle = NULL;
    // xTaskCreatePinnedToCore(VgaFrameTask, "vga", 10000, NULL, 5, &xHandle, 0);
}

void DibujarSol(float PosX, float PosY, float RadioSol, float RadioNucleo)
{
    float x1, x2, x3, x4, y1, y2, y3, y4;
    vga.fillCircle(PosX + RadioSol, PosY + RadioSol, RadioNucleo, AMARILLO);
    vga.line(PosX, PosY + RadioSol, PosX + RadioSol + RadioSol, PosY + RadioSol, AMARILLO);
    vga.line(PosX + RadioSol, PosY, PosX + RadioSol, PosY + RadioSol + RadioSol, AMARILLO);

    x3 = x1 = PosX + RadioSol - (0.707 * RadioSol);
    x2 = x4 = PosX + RadioSol + (0.707 * RadioSol);

    y1 = y2 = PosY + RadioSol - (0.707 * RadioSol);
    y3 = y4 = PosY + RadioSol + (0.707 * RadioSol);

    vga.line(x1, y1, x4, y4, AMARILLO);
    vga.line(x2, y2, x3, y3, AMARILLO);
}

void DibujarNube(int x, int y)
{
    vga.fillCircle(x + 10, y + 10, 10, CELESTE);
    // vga.fillCircle(x + 12, y + 8, 13, CELESTE);
    vga.fillCircle(x + 20, y + 9, 11, CELESTE);

}

void Pantalla1()
{
    vga.fillRect(0, 0, ancho, alto, vga.RGB(0, 0, 64));
    // <--- dibujar primero el fondo

    vga.setFont(CodePage437_9x16);  // TITULO
    vga.setCursor(ancho / 2 - 30, 0);
    vga.setTextColor(vga.RGB(64, 128, 255));
    vga.print("ISET 57");

    // ultimos datos leidos x el sensor:
    vga.setCursor(30, 50);
    vga.setTextColor(vga.RGB(128, 0, 255));
    vga.print("Temperatura: ");
    vga.print((long)temperature);
    vga.println(" *C");
    vga.print("Humedad: ");
    vga.print((long)humidity);
    vga.println(" %");
    vga.rect(8, 125, 304, 105, vga.RGB(255, 2, 64));
}

void DibujarLluvia(int x, int y) {}

char *DIAS[] = {"DOMINGO", "LUNES", "MARTES", "MIERCOLES", "JUEVES", "VIERNES", "SABADO"};

void Pantalla2()
{
    vga.fillRect(0, 0, ancho, alto, vga.RGB(0, 64, 64));

    time_t seg    = time(0)+24*3600;
    struct tm *tm = localtime(&seg);
    char fecha[100];
    sprintf(fecha, "PRONOSTICO %s %d", DIAS[tm->tm_wday], tm->tm_mday);

    vga.setFont(CodePage437_9x16);  // TITULO
    vga.setCursor(ancho / 2 - 100, 0);
    vga.setTextColor(vga.RGB(255, 64, 0));
    vga.print(fecha);

    vga.rect(8, 20, 95, 85, vga.RGB(64, 255, 64)); //10hs
    vga.rect(111, 20, 95, 85, vga.RGB(64, 255, 64)); //13hs
    vga.rect(214, 20, 95, 85, vga.RGB(64, 255, 64)); //20hs
    vga.rect(8, 150, 301, 75, vga.RGB(64, 255, 64));  // abajo

    vga.setFont(CodePage437_8x8);
    vga.setTextColor(BLANCO);

    if (precipitation_probability[0] > 20) DibujarLluvia(30, 30);
    if (cloudcover[0] < 66) DibujarSol(11, 22, 15, 9);
    if (cloudcover[0] > 33) DibujarNube(30, 25);
    vga.setCursor(10, 63);
    vga.print("Nubes ");
    vga.print((int)cloudcover[0]);
    vga.print("%");
    vga.setCursor(10, 75);
    vga.print("Temp ");
    vga.print((int)temperature_2m[0]);
    vga.print("*C");
    vga.setCursor(10, 87);
    vga.print("Lluvia ");
    vga.print((int)precipitation_probability[0]);
    vga.print("%");
    vga.setCursor(40, 110);
    vga.print("10Hs");

    if (precipitation_probability[1] > 20) DibujarLluvia(217, 30);
    if (cloudcover[1] < 66) DibujarSol(114, 22, 15, 9);
    if (cloudcover[1] > 33) DibujarNube(144, 25);
    vga.setCursor(113, 63);
    vga.print("Nubes ");
    vga.print((int)cloudcover[1]);
    vga.print("%");
    vga.setCursor(113, 75);
    vga.print("Temp ");
    vga.print((int)temperature_2m[1]);
    vga.print("*C");
    vga.setCursor(113, 87);
    vga.print("Lluvia ");
    vga.print((int)precipitation_probability[1]);
    vga.print("%");
    vga.setCursor(143, 110);
    vga.print("13Hs");

    if (precipitation_probability[2] > 20) DibujarLluvia(247, 30);
    if (cloudcover[2] < 66) DibujarSol(217, 22, 15, 9);
    if (cloudcover[2] > 33) DibujarNube(247, 25);
    vga.setCursor(216, 63);
    vga.print("Nubes ");
    vga.print((int)cloudcover[2]);
    vga.print("%");
    vga.setCursor(216, 75);
    vga.print("Temp ");
    vga.print((int)temperature_2m[2]);
    vga.print("*C");
    vga.setCursor(216, 87);
    vga.print("Lluvia ");
    vga.print((int)precipitation_probability[2]);
    vga.print("%");
    vga.setCursor(246, 110);
    vga.print("20Hs");

    // mostrar textos
    // temperature_2m[0]

    vga.setCursor(10, 180);
    vga.print(TextoTelegram);
}

void Pantalla3()
{
    vga.fillRect(0, 0, ancho, alto, vga.RGB(0, 64, 64));
    vga.rect(8, 20, 95, 95, vga.RGB(64, 255, 64));
    vga.rect(111, 20, 95, 95, vga.RGB(64, 255, 64));
    vga.rect(214, 20, 95, 95, vga.RGB(64, 255, 64));
    vga.rect(8, 125, 304, 105, vga.RGB(64, 255, 64));
}

// aca se arma el frame para el VGA:
void ActualizarPantallas()
{
    static int pantallaActiva = 0;
    static long T             = 1000;
    if (millis() > T)
    {
        T = millis() + 10000;
        pantallaActiva++;
        if (pantallaActiva == 3) pantallaActiva = 0;
    }

    // if (pantallaActiva == 0) Pantalla1();
    if (pantallaActiva == 1) Pantalla2();
    // if (pantallaActiva == 2) Pantalla3();

    // show the backbuffer (only needed when using backbuffering)
    vga.show();
    delay(20);
}
