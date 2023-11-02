// This example shows a rendering of Julia set. Please change the pinConfig if you are using a different board to PicoVGA
// cc by-sa 4.0 license
// bitluni

// include libraries
#include <ESP32Lib.h>
#include <SimpleDHT.h>
#include <Ressources/CodePage437_8x19.h>
#include <Ressources/CodePage437_9x16.h>

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 23;
SimpleDHT11 dht11(pinDHT11);
int temperatura;
int humedad;

VGA6Bit vga;
const PinConfig &pinConfig = VGA6Bit::PicoVGA;

int taskData[2][3] = {
    {0, 0, 160},
    {0, 160, 320},
};

static float v = -1.5;
static float vs = 0.0005;

// https://en.wikipedia.org/wiki/Julia_set#Pseudocode_for_normal_Julia_sets
int julia(int x, int y, float cx, float cy)
{
  int zx = ((x - 159.5f) * (1.f / 320.f * 5.0f)) * (1 << 12);
  int zy = ((y - 99.5f) * (1.f / 200.f * 3.0f)) * (1 << 12);
  int i = 0;
  const int maxi = 17;
  int cxi = cx;
  int cyi = cy * (1 << 12);
  while (zx * zx + zy * zy < (4 << 24) && i < maxi)
  {
    int xtemp = (zx * zx - zy * zy) >> 12;
    zy = ((zx * zy) >> 11) + cyi;
    zx = xtemp + cxi;
    i++;
  }
  return i;
}

int colors[] = {
    0b110001,
    0b110010,
    0b110011,
    0b100011,
    0b010011,
    0b000011,
    0b000111,
    0b001011,
    0b001111,
    0b001110,
    0b001101,
    0b001100,
    0b011100,
    0b101100,
    0b111100,
    0b111000,
    0b110100,
    0b110000,
};

void renderTask(void *param)
{
  int *data = (int *)param;
  while (true)
  {
    while (!data[0])
      delay(1);
    for (int y = 0; y < 100; y++)
      for (int x = data[1]; x < data[2]; x++)
      {
        int c = colors[julia(x, y, -0.74543f, v)];
        vga.dotFast(x, y, c);
        vga.dotFast(319 - x, 199 - y, c);
      }
    data[0] = 0;
  }
}

void ActualizarFractal()
{

  static unsigned long ot = 0;
  unsigned long t = millis();
  unsigned long dt = t - ot;
  ot = t;
  taskData[0][0] = 1;
  taskData[1][0] = 1;

  // waiting for task to finish
  while (taskData[0][0] || taskData[1][0])
    delay(1);

  v += vs * dt;
  if (v > 1.5f)
  {
    v = 1.5f;
    vs = -vs;
  }
  if (v < -1.5f)
  {
    v = -1.5f;
    vs = -vs;
  }
}

// initial setup
void setup()
{
  Serial.begin(115200);

  // estos parametros son para mi monitor (javier)
  // 360*480
  // Mode MiMonitor(8, 54, 28, 360, 11, 2, 32, 480, 2, 14161000, 1, 1);
  // vga.init(MiMonitor, pinConfig);

  // 320*400
  // vga.init(vga.MODE320x400, pinConfig); //media pantalla

  // 320*400
  // vga.init(vga.MODE320x200, pinConfig); // ok, muy bien

  // 320*480
  // vga.init(vga.MODE320x240, pinConfig); // ok, muy bien

  // 360*400
  vga.init(vga.MODE360x200, pinConfig); // ok, se puede ajustar el monitor manual, el autoajuste lo desajusta.

  // taskData[0][0] = 0;
  // taskData[0][1] = 0;
  // taskData[0][2] = vga.mode.vRes / 2;
  // taskData[1][0] = 0;
  // taskData[1][1] = vga.mode.vRes / 2;
  // taskData[1][2] = vga.mode.vRes;

  TaskHandle_t xHandle = NULL;
  xTaskCreatePinnedToCore(renderTask, "Render1", 2000, taskData[0], (2 | portPRIVILEGE_BIT), &xHandle, 0);
  xTaskCreatePinnedToCore(renderTask, "Render2", 2000, taskData[1], (2 | portPRIVILEGE_BIT), &xHandle, 1);

  Serial.print("VGA Inicializado: ");
  Serial.print(vga.mode.hRes);
  Serial.print(" x ");
  Serial.println(vga.mode.vRes);
}

void LeerTemperatura()
{
  static unsigned long T = 0;
  if (millis() > T)
  {
    T = millis() + 1000;
    byte temperature = 0;
    byte humidity = 0;
    if (dht11.read(&temperature, &humidity, NULL) == 0)
    {
      temperatura = temperature;
      humedad = humidity;
      Serial.print("DHT11 OK: ");
      Serial.print((int)temperature);
      Serial.print(" *C, ");
      Serial.print((int)humidity);
      Serial.println(" H");
    }
  }
}

// just draw each frame
void loop()
{
  ActualizarFractal();
  // LeerTemperatura();

  // vga.setFont(CodePage437_8x19);
  // vga.setTextColor(vga.RGB(255, 128, 0), vga.RGB(0, 0, 255));
  // vga.setCursor(20, 20);
  // vga.print("TEMPERATURA ");
  // vga.print(temperatura);
  // vga.print("°C   HUMEDAD ");
  // vga.print(humedad);
  // vga.print(" %");
  // vga.show();
}
