// JAVIER 2023

#include <ESP32Lib.h>
#include "MiVga6.h"

MiVGA6 vga;

const int Channel1Pin = 34;
int DefinicionY;         // se calcula segun la altura del monitor y el valor del ADC
int AjusteVertical = 0;  // se podria mover con un pote
#define CC 0             // se monta la onda sobre una CC
#define CA 1             // alterna (esta centrado)

void setup()
{
    Serial.begin(115200);
    vga.Inicializar(1);  // cuantos frames?
    DefinicionY = 2 + 4096 / vga.alto;
}

void marco()
{
    vga.rect(0, 0, vga.ancho, vga.alto, BLANCO);
    vga.rect(1, 1, vga.ancho - 2, vga.alto - 2, GRIS);
    vga.rect(2, 2, vga.ancho - 4, vga.alto - 4, AZULITO);
}

void titulo()
{
    vga.setFont(CodePage437_8x19);
    vga.setTextColor(ROJO, NEGRO);
    vga.PrintCenterScreen(-1, 5, "OSCILOSCOPIO");
}

void loop()
{
    // vga.clear();
    static int X           = 0;  // pos del plot
    static unsigned long T = 0;

    // marco();
    // titulo();
    static int adcAnt = 0;
    int adc           = analogRead(Channel1Pin);       // 0-4096
    int value         = vga.alto - adc / DefinicionY;  // calculo el paso segun el monitor
                                                       // if (CC)  adc = adc - AjusteVertical;       // Ajuste vertical.
                                                       // adc = 100;

                                                       // if (millis() > T)
    // {
    // T = millis() + 1;  // tiempo de muestreo
    X++;
    // vga.dot(X, adc, VERDE);
    vga.line(X - 1, adcAnt, X, adc, VERDE);
    adcAnt = adc;
    if (X > vga.ancho)
    {
        X = 0;
        vga.clear();
    }
    // }

    vga.show();
}
