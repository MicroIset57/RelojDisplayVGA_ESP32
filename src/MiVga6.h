#pragma once
#include <ESP32Lib.h>
#include <Ressources/CodePage437_8x8.h>
#include <Ressources/CodePage437_8x14.h>
#include <Ressources/CodePage437_8x16.h>
#include <Ressources/CodePage437_8x19.h>
#include <Ressources/CodePage437_9x16.h>
#include <Ressources/Font6x8.h>
#include <Ressources/Font8x8.h>

#define COLOR_RGB(r, g, b) (((r >> 6) & 0b11) | ((g >> 4) & 0b1100) | ((b >> 2) & 0b110000) | 0b11000000)

#define NEGRO              COLOR_RGB(0, 0, 0)
#define BLANCO             COLOR_RGB(255, 255, 255)
#define GRIS_CLARO         COLOR_RGB(128, 128, 128)
#define GRIS               COLOR_RGB(64, 64, 64)
#define ROJO               COLOR_RGB(255, 0, 0)  // Colores standar
#define VERDE              COLOR_RGB(0, 255, 0)
#define AZUL               COLOR_RGB(0, 0, 255)
#define CIAN               COLOR_RGB(0, 255, 255)
#define AMARILLO           COLOR_RGB(255, 255, 0)
#define ROSA               COLOR_RGB(255, 0, 255)
#define ROJITO             COLOR_RGB(128, 0, 0)  // Buenooooo
#define VERDITO            COLOR_RGB(0, 128, 0)
#define AZULITO            COLOR_RGB(0, 0, 128)
#define CELESTE            COLOR_RGB(64, 128, 255)
#define CIANITO            COLOR_RGB(0, 128, 128)
#define AMARILLITO         COLOR_RGB(128, 128, 0)
#define ROSITA             COLOR_RGB(128, 0, 128)

class MiVGA6 : public VGA6Bit
{
   private:
    const PinConfig &pinConfig = MiVGA6::PicoVGA;

   public:
    int ancho, alto;  // tamaño del display

    void Inicializar(int CantFrames)
    {
        setFrameBufferCount(CantFrames);

        // estos parametros son para mi monitor (javier)
        // 360*480
        //  Mode MiMonitor(8, 54, 28, 360, 11, 2, 32, 480, 2, 14161000, 1, 1);
        // 360 x 240
        // Mode MiMonitor(8, 54, 28, 360, 11, 2, 32, 240, 1, 14161000, 1, 1);
        // vga.init(MiMonitor, pinConfig);
        // 320*400
        // vga.init(vga.MODE320x400, pinConfig); //media pantalla
        // 320*400
        // vga.init(vga.MODE320x200, pinConfig); // ok, muy bien
        // 320*480
        // vga.init(vga.MODE320x240, pinConfig); // ok, muy bien
        // 360*400
        init(MODE360x200, pinConfig);  // ok, se puede ajustar el monitor manual, el autoajuste lo desajusta.

        ancho = mode.hRes - 1;  // lo ajusto porque no esta bien
        alto  = mode.vRes / 2 - 1;
        Serial.printf("VGA Inicializado: %d x %d\n", ancho, alto);
        clear();
    }

    // si x o y es -1 se centra en la pantalla
    void PrintCenterScreen(int x, int y, const char *texto)
    {
        int w, h;
        GetStringSize(texto, &h, &w);
        int X = x;
        int Y = y;
        if (x < 0) X = (ancho - w) / 2;  // centrado
        if (y < 0) Y = (alto - h) / 2;
        setCursor(X, Y);
        print(texto);
    }

    void GetStringSize(const char *texto, int *alto, int *ancho)
    {
        *ancho = *alto = 0;
        if (!font) return;
        char *str = (char *)texto;
        int max = 0, w = 0, i = 0, len = strlen(str);
        *alto = font->charHeight;  // alto de primera linea
        while (*str)
        {
            ++i;
            if (*str == '\n')
            {
                if (max < w) max = w;
                // si termina en ENTER, esa linea no la cuento.
                if (i < len) *alto += font->charHeight;
            }
            else
                w += font->charWidth;
            str++;
        }
        *ancho = (max == 0 ? w : max);

        // Serial.printf("{%s} => Ancho:%d Alto:%d\n", texto, *ancho, *alto);
    }
};
