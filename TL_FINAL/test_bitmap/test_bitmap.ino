#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

#define TFT_CLK 13
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8

MCUFRIEND_kbv tft;
int16_t g_identifier;

const int tamanioAlien = 8;
const int tamanioNave = 9;

const byte naveBytes[tamanioAlien][tamanioNave] PROGMEM = {
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,1,0,0,0,0},
  {0,0,0,1,1,1,0,0,0},
  {0,0,1,1,1,1,1,0,0},
  {0,0,1,1,1,1,1,0,0},
  {1,1,1,1,1,1,1,1,1}
};


static const unsigned char PROGMEM InvaderMiddleGfx2 [] = {
  B00100000,B10000000,
  B00010001,B00000000,
  B10111111,B10100000,
  B10101110,B10100000,
  B11111111,B11100000,
  B00111111,B10000000,
  B00100000,B10000000,
  B01000000,B01000000
};

const unsigned char InvaderTopGfx [] PROGMEM = {
  B00011000,
  B00111100,
  B01111110,
  B11011011,
  B11111111,
  B00100100,
  B01011010,
  B10100101
};

const unsigned char alienFilaUno[] PROGMEM = {
  B00000011,B11000000,
  B00000011,B11000000,
  B00111111,B11111100,
  B00111111,B11111100,
  B11111111,B11111111,
  B11111111,B11111111,
  B11000011,B11000011,
  B11000011,B11000011,
  B11111111,B11111111,
  B11111111,B11111111,
  B00001100,B00110000,
  B00001100,B00110000,
  B00110011,B11001100,
  B00110011,B11001100,
  B11000000,B00000011,
  B11000000,B00000011
};

uint8_t* punteroNave = (uint8_t*)naveBytes;


void setup() {
  tft.begin();
  g_identifier = tft.readID();
  Serial.begin(9600);
  //Serial.print("ID = 0x");
  //Serial.println(g_identifier, HEX);
  tft.begin(g_identifier);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  // Supongamos que tienes una matriz de 8x8 almacenada en un array llamado "matriz"
  // Cada elemento en "matriz" representa un píxel en la pantalla
  int x = 30;
  int direccion = 1;
  while(true){
    
    //drawInvader();
    tft.drawBitmap(x, 50, alienFilaUno, 16, 16, TFT_BLACK);
    //delay(1000);
    if(x > tft.width() - 216){
      direccion = -1;
    }
    else if(x < 0) {
      direccion = 1;
    }
    if (direccion == 1){
      x += 1;
    }
    else {
      x -= 1;
    }
    //delay(500);
    Serial.print(x);
    tft.drawBitmap(x, 50, alienFilaUno, 16, 16, TFT_WHITE);
    //delay(1000);
    /*
    for (int i = 0; i < sizeof(alienFilaUno); i++) {
    uint8_t currentByte = pgm_read_byte(&alienFilaUno[i]);
    for (int j = 0; j < 8; j++) {
        if (currentByte & (1 << j)) {
          // Draw a pixel on the screen
          int x = (i % 11) * 32 + j;
          int y = (i / 11) * 32;
          tft.drawPixel(50+j, 10+i, TFT_WHITE);
        }
      }
    }*/
  }
}

// function to draw a single alien
void drawInvader() {
 for (int i = 0; i < 8; i++) {
    uint8_t currentByte = pgm_read_byte(&InvaderTopGfx[i]);
    Serial.println(currentByte);
    delay(1000);
    for (int j = 0; j < 8; j++) {
      if (currentByte & (1 << j)) {
        // Draw a pixel on the screen
        int x = (i % 11) * 32 + j;
        int y = (i / 11) * 32;
        tft.drawPixel(x, y, TFT_WHITE);
      }
    }
  }
}
