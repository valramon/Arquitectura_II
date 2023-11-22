#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h> //libreria de la pantalla
#include <MemoryFree.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
//#define TFT_BROWN 0x1A0A

MCUFRIEND_kbv tft;
int16_t g_identifier;
int buzzer = 53;

int xPin = A8;
int buttonPin = 39;
int tiempoDelay = 2000;

const int tamanioAlien = 8;
const int tamanioNave = 9;

const int aliensPorFila = 11;
const int filasAliens = 5;

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
  B01100000,B00000110,
  B01100000,B00000110
};

const unsigned char alienFilaUnoAnimado[] PROGMEM = {
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
  B00110000,B00001100,
  B00110000,B00001100
};

const unsigned char alienFilaDos[] PROGMEM = {
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
  B00110011,B11001100,
  B00110011,B11001100,
  B00110000,B00001100,
  B00110000,B00001100,
  B00001111,B11110000,
  B00001111,B11110000
};

const unsigned char alienFilaDosAnimado[] PROGMEM = {
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
  B00110011,B11001100,
  B00110011,B11001100,
  B00110000,B00110000,
  B00110000,B00110000,
  B00111111,B11000000,
  B00111111,B11000000
};

const unsigned char alienFilaTres[] PROGMEM = {
  B00001100,B00110000,
  B00001100,B00110000,
  B00000011,B11000000,
  B00000011,B11000000,
  B00001111,B11110000,
  B00001111,B11110000,
  B00110011,B11001100,
  B00110011,B11001100,
  B11111111,B11111111,
  B11111111,B11111111,
  B11001111,B11110011,
  B11001111,B11110011,
  B00000110,B01100000,
  B00000110,B01100000,
  B00011000,B00011000,
  B00011000,B00011000
};

const unsigned char alienFilaTresAnimado[] PROGMEM = {
  B00001100,B00110000,
  B00001100,B00110000,
  B00000011,B11000000,
  B00000011,B11000000,
  B11001111,B11110011,
  B11001111,B11110011,
  B11110011,B11001111,
  B11110011,B11001111,
  B00111111,B11111100,
  B00111111,B11111100,
  B00001111,B11110000,
  B00001111,B11110000,
  B00001100,B00110000,
  B00001100,B00110000,
  B00000110,B01100000,
  B00000110,B01100000
};

const unsigned char alienFilaCuatro[] PROGMEM = {
  B00001100,B00110000,
  B00001100,B00110000,
  B00000011,B11000000,
  B00000011,B11000000,
  B11001111,B11110011,
  B11001111,B11110011,
  B11110011,B11001111,
  B11110011,B11001111,
  B11111111,B11111111,
  B11111111,B11111111,
  B11001111,B11110011,
  B11001111,B11110011,
  B11000011,B11000011,
  B11000011,B11000011,
  B11110000,B00001111,
  B11110000,B00001111
};

const unsigned char alienFilaCuatroAnimado[] PROGMEM = {
  B00001100,B00110000,
  B00001100,B00110000,
  B11000011,B11000011,
  B11000011,B11000011,
  B11001111,B11110011,
  B11001111,B11110011,
  B11110011,B11001111,
  B11110011,B11001111,
  B11111111,B11111111,
  B11111111,B11111111,
  B11001111,B11110011,
  B11001111,B11110011,
  B11110011,B11001111,
  B11110011,B11001111,
  B00000000,B00000000,
  B00000000,B00000000
};

const unsigned char alienFilaCinco[] PROGMEM = {
B00000001,B10000000,
B00000001,B10000000,
B00000111,B11100000,
B00001111,B11110000,
B00011111,B11111000,
B00111111,B11111100,
B11100001,B10000111,
B11100001,B10000111,
B11111111,B11111111,
B11111111,B11111111,
B00001111,B11110000,
B00001111,B11110000,
B00110001,B10001100,
B00110001,B10001100,
B11000110,B01100011,
B11000110,B01100011
};

const unsigned char alienFilaCincoAnimado[] PROGMEM = {
B00000001,B10000000,
B00000001,B10000000,
B00000111,B11100000,
B00001111,B11110000,
B00011111,B11111000,
B00111111,B11111100,
B11100001,B10000111,
B11100001,B10000111,
B11111111,B11111111,
B11111111,B11111111,
B00001111,B11110000,
B00001111,B11110000,
B00110111,B11101100,
B00110111,B11101100,
B00110000,B00001100,
B00110000,B00001100
};

//const int *spritesInvaders[] = {&alienFilaUno[0][0], &alienFilaDos[0][0], &alienFilaTres[0][0], &alienFilaCuatro[0][0], &alienFilaCinco[0][0]};
const uint16_t coloresAliens[] = {TFT_CYAN, TFT_PURPLE, TFT_DARKGREEN, TFT_RED, TFT_ORANGE};
const unsigned char *spritesInvaders[] = {alienFilaUno, alienFilaDos, alienFilaTres, alienFilaCuatro, alienFilaCinco};
const unsigned char *spritesInvadersAnimados[] = {alienFilaUnoAnimado, alienFilaDosAnimado, alienFilaTresAnimado, alienFilaCuatroAnimado, alienFilaCincoAnimado};

const unsigned char naveSprite[] PROGMEM = {
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000001,B10000000,
  B00000001,B10000000,
  B00000011,B11000000,
  B00000011,B11000000,
  B00001111,B11110000,
  B00001111,B11110000,
  B00001111,B11110000,
  B00001111,B11110000,
  B00111111,B11111100,
  B00111111,B11111100
};

static const unsigned char PROGMEM ExplosionGfx [] = {
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00001000,B10000000,
  B01000101,B00010000,
  B00100000,B00100000,
  B00010000,B01000000,
  B11000000,B00011000,
  B00010000,B01000000,
  B00100101,B00100000,
  B01001000,B10010000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
};

class Nave {
  public:
    int vidas;
    int x;
    int y;
    int x_fin = 0;
    int y_fin = 0;
    int tiempo = millis();
    // sets values for private date members x and y
    Nave() {
      x = 0;
      y = 0;
    }
    Nave(int x_arg, int y_arg, int vidas_arg) {
      x = x_arg;
      y = y_arg;
      x_fin = x_arg + (7 * 2);
      y_fin = y_arg + (7 * 2);
      vidas = vidas_arg;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    void drawNave(int borrado) {
      int i;
      int j;
      if (borrado == -1){
        tft.drawBitmap(x, y, naveSprite, 16, 16, TFT_BLACK);
      }
      else {
        tft.drawBitmap(x, y, naveSprite, 16, 16, TFT_WHITE);
      }
      x_fin = x + 16;
      y_fin = y + 16;
    }
};

class Cannonball {
  public:
    int x;
    int y;
    unsigned long tiempo;
    Cannonball(){
      x = 0;
      y = 0;
    }
    Cannonball(int x_arg, int y_arg, unsigned long tiempo_arg){
      x = x_arg;
      y = y_arg;
      tiempo = tiempo_arg;
    }
    void draw(){
      tft.fillRect(x, y + 2, 2, 2, TFT_BLACK);
      tft.fillRect(x, y, 2, 2, TFT_WHITE);
      tft.fillRect(x, y - 2, 2, 2, TFT_WHITE);
    }
    void update(){
      y -= 2;
    }
    void explode(){
      tft.fillRect(x, y + 2, 2, 2, TFT_BLACK);
      tft.fillRect(x, y, 2, 2, TFT_BLACK);
      tft.drawBitmap(x, y, ExplosionGfx, 16, 16, TFT_RED);
      tone(buzzer, 100, 100);
      delay(100);
      tft.drawBitmap(x, y, ExplosionGfx, 16, 16, TFT_BLACK);
    }
};

class Invader {
  public:
    uint16_t color;
    int nivelFila;
    int vida;
    int x;
    int y;
    int x_fin = 0;
    int y_fin = 0;
    unsigned long tiempo;
    // Constructors
    Invader() {
      x = 0;
      y = 0;
    }
    // sets values for private date members x and y
    Invader(int x_arg, int y_arg, int nivelFila_arg, int vida_arg, uint16_t color_arg) {
      x = x_arg;
      y = y_arg;
      nivelFila = nivelFila_arg;
      vida = vida_arg;
      color = color_arg;
      x_fin = x_arg + (7 * 2);
      y_fin = y_arg + (7 * 2);
    }
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    void drawSpaceInvader(int borrado, bool animacion) {
      if(borrado){
        tft.drawBitmap(x, y, spritesInvaders[nivelFila], 16, 16, color);/*
        if(animacion){
          tft.drawBitmap(x, y, spritesInvadersAnimados[nivelFila], 16, 16, color);
        }
        else{
          tft.drawBitmap(x, y, spritesInvaders[nivelFila], 16, 16, color);
        }*/
      }
      else {
        tft.drawBitmap(x, y, spritesInvaders[nivelFila], 16, 16, TFT_BLACK);/*
        if(animacion){
          tft.drawBitmap(x, y, spritesInvadersAnimados[nivelFila], 16, 16, TFT_BLACK);
        }
        else{
          tft.drawBitmap(x, y, spritesInvaders[nivelFila], 16, 16, TFT_BLACK);
        }*/
      }
    }
    void explode(){
      tft.drawBitmap(x, y, spritesInvaders[nivelFila], 16, 16, TFT_BLACK);
      tft.drawBitmap(x, y, ExplosionGfx, 16, 16, TFT_WHITE);
      tone(buzzer, 50, 100);
      delay(100);
      tft.drawBitmap(x, y, ExplosionGfx, 16, 16, TFT_BLACK);
    }
    void eliminarSpaceInvader(){
      vida -= 1;
      if (vida == 0){
        //drawSpaceInvader(0);
        explode();
      }
    }
};

class Juego {
  public:
    configurar(){
      generarAliens(invaders);
      tft.setTextSize(1);
      tft.setCursor(10, 450);
      tft.print("Nivel: ");
      tft.setCursor(50, 450);
      tft.print(nivel, 10);
      tft.setCursor(120, 450);
      tft.print("Puntaje: ");
      tft.setCursor(200, 450);
      tft.print(puntaje, 10);
    }
    void drawAliens(int borrado) {
      for (int i = 0; i < filasAliens; i++) {
        for (int j = 0; j < aliensPorFila; j++) {
          invaders[i][j].drawSpaceInvader(borrado, false);
        }
      }
    }
    void drawNave(int borrado){
      nave.drawNave(borrado);
    }
    void moverNave(){
      int valorX = analogRead(xPin);
      if(millis() - nave.tiempo >= 100){
        if(valorX > 600){
          if(nave.x_fin <= tft.width()){
            nave.drawNave(-1);
            nave.x += 2;
            nave.drawNave(1);
          }
        }
        else if (valorX < 450) {
          if(nave.x - 2 >= 0){
            nave.drawNave(-1);
            nave.x -= 2;
            nave.drawNave(1);
          }
        }
      }
      //delay(10);
    }
    void disparaNave(){
      if (!digitalRead(buttonPin) && !hayDisparo){
        cannonball = Cannonball(nave.x + 3 * 2, nave.y - 2, millis());
        hayDisparo = 1;
        tone(buzzer, 1000, 100);
      }
      if(hayDisparo){
        updateCannonball();
      }
    }

    void updateCannonball(){
      if(millis() - cannonball.tiempo >= 3){
        cannonball.tiempo = millis();
        cannonball.draw();
        cannonball.update();
        eliminaAlien();
        if(cannonball.y - 2 <= topeSuperior - 16){
          hayDisparo = 0;
          cannonball.explode();
        }
      }
    }
    void updateAliens(){
      topeSuperior = invaders[0][0].y;
      bool puedeMover = puedeMoverAliens();
      if(puedeMover){
        desplazarAliens(direccion);
        filaAMover += 1;
        if(filaAMover >= 5){
          filaAMover = 0;
          if(!cambioAnimacion){
            animacion = false;
            cambioAnimacion = true;
          }
          //animacion = !animacion;
          //cambioAnimacion = true;
        }
      }
      else{
        while(filaAMover < 5){
          desplazarAliens(direccion);
          filaAMover += 1;
        }
        for (int i = 0; i < filasAliens; i++) {
          for(int j = 0; j < aliensPorFila; j++){
            if(invaders[i][j].vida > 0){
              if(cambioAnimacion){
                invaders[i][j].drawSpaceInvader(0, false);
                cambioAnimacion = false;
              }
              else{
                invaders[i][j].drawSpaceInvader(0, animacion);
              }
              invaders[i][j].y+=10;
              invaders[i][j].y_fin+=10;
              moverNave();
              if(hayDisparo){
                updateCannonball();
              }
              invaders[i][j].drawSpaceInvader(1, animacion);
              if (invaders[i][j].y + 10 >= nave.y){
                esGameOver = true;
              }
            }
          }
        }
        direccion = !direccion;
        filaAMover += 1;
        if(filaAMover >= 5){
          filaAMover = 0;
          if(!cambioAnimacion){
            animacion = false;
            cambioAnimacion = true;
          }
        }
      }
      if(cantidadEnemigos == 0){
        subeNivel();
      }
    }
    void pantallaInicio(){
      tft.setCursor(120, 100);
      tft.setTextColor(TFT_DARKGREEN);
      tft.print("Space Invaders");
      while(digitalRead(buttonPin)){}
      tft.fillScreen(TFT_BLACK);
    }
    void gameOver(){
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(120, 100);
      tft.setTextColor(TFT_DARKGREEN);
      tft.print("GAME OVER!");
      tft.drawBitmap(120, 200, naveSprite, 16, 16, TFT_WHITE);
      while(digitalRead(buttonPin)){
        tft.drawBitmap(120, 200, naveSprite, 16, 16, TFT_BLACK);
        tft.drawBitmap(120, 200, ExplosionGfx, 16, 16, TFT_WHITE);
        delay(1000);
        tft.drawBitmap(120, 200, ExplosionGfx, 16, 16, TFT_BLACK);
        tft.drawBitmap(120, 200, naveSprite, 16, 16, TFT_WHITE);
        delay(1000);
      }
      esGameOver = false;
      tft.fillScreen(TFT_BLACK);
      reiniciar();
    }
    void reiniciar(){
      pantallaInicio();
      updateLevel();
      delay(2000);
      tft.fillScreen(TFT_BLACK);
      configurar();
      drawAliens(1);
      drawNave(1);
      tiempoDelay = 2000;
    }
    bool get_gameOver(){
      return esGameOver;
    }
    void subeNivel(){
      nivel += 1;
      cantidadEnemigos = 55;
      direccion = true;
      columnaDerechaViva = 0;
      columnaIzquierdaViva = aliensPorFila - 1;
      topeDerecha[0] = aliensPorFila - 1;
      topeDerecha[1] = 0;
      topeIzquierda[0] = 0;
      topeIzquierda[1] = 0;
      topeSuperior = 0;
      filaAMover = 0;
      updateLevel();
      delay(2000);
      tft.fillScreen(TFT_BLACK);
      configurar();
      drawAliens(1);
      drawNave(1);
      tiempoDelay = 2000;
    }
  private:
    int nivel = 1;
    int cantidadEnemigos = 55;
    int puntaje = 0;
    int factorPuntaje = 10;
    bool direccion = true;
    bool animacion = false;
    bool cambioAnimacion = false;
    int columnaDerechaViva = 0;
    int columnaIzquierdaViva = aliensPorFila - 1;
    int topeDerecha[2] = {aliensPorFila - 1, 0};
    int topeIzquierda[2] = {0, 0};
    int topeSuperior = 0;
    int filaAMover = 0;
    bool esGameOver = false;
    unsigned long tiempoAliens = millis();
    Invader invaders[filasAliens][aliensPorFila];
    Nave nave = Nave(150, 420, 3);
    Cannonball cannonball;
    int hayDisparo = 0;
    void generarAliens(Invader invaders[filasAliens][aliensPorFila]){
      int vida = 5;
      for(int fila = 0; fila < filasAliens; fila++){
        for(int cantAliens = 0; cantAliens < aliensPorFila; cantAliens++){
            invaders[fila][cantAliens] = Invader(30 + (25*cantAliens), 10 + (30 * fila), fila, vida - fila, coloresAliens[fila]);
        }
      }
    }
    void eliminaAlien(){
      bool nuevoTope = false;
      for(int fila = 0; fila < filasAliens; fila++){
        for(int cantAliens = 0; cantAliens < aliensPorFila; cantAliens++){
            if((cannonball.x - 1 >= invaders[fila][cantAliens].x && cannonball.x - 1 < invaders[fila][cantAliens].x_fin) && (cannonball.y - 4 <= invaders[fila][cantAliens].y_fin) && invaders[fila][cantAliens].vida > 0){
              invaders[fila][cantAliens].eliminarSpaceInvader();
              cannonball.explode();
              hayDisparo = 0;
              if(invaders[fila][cantAliens].vida == 0){
                cantidadEnemigos -= 1;
                puntaje += ((filasAliens - invaders[fila][cantAliens].nivelFila) * factorPuntaje);
                if(direccion){
                  if(topeDerecha[0] == cantAliens){
                    int i = cantAliens;
                    int j = 0;
                    while(!nuevoTope){
                      if(invaders[j][i].vida > 0){
                        topeDerecha[0] = i;
                        topeDerecha[1] = j;
                        nuevoTope = true;
                      }
                      j += 1;
                      if (j >= filasAliens){
                        j = 0;
                        i -= 1;
                      }
                    }
                  }
                }
                else{
                  if(topeIzquierda[0] == cantAliens){
                    int i = cantAliens;
                    int j = 0;
                    while(!nuevoTope){
                      if(invaders[j][i].vida > 0){
                        topeIzquierda[0] = i;
                        topeIzquierda[1] = j;
                        nuevoTope = true;
                      }
                      j += 1;
                      if (j >= filasAliens){
                        j = 0;
                        i += 1;
                      }
                    }
                  }
                }
                tft.fillRect(200, 420, 100, 60, TFT_BLACK);
                tft.setTextColor(TFT_WHITE);
                tft.setCursor(200, 450);
                tft.print(puntaje, 10);
              }
              break;
            }
          
        }
      }
    }
    void desplazarAliens(bool direccion){
      for (int j = 0; j < aliensPorFila; j++) {
        if(invaders[filaAMover][j].vida > 0){
          if(cambioAnimacion){
            invaders[filaAMover][j].drawSpaceInvader(0, !animacion);
            cambioAnimacion = false;
            Serial.println("entre cambio");
            Serial.println(animacion);
          }
          else{
            invaders[filaAMover][j].drawSpaceInvader(0, animacion);
          }
          if(direccion){
            invaders[filaAMover][j].x+=10;
          }
          else{
            invaders[filaAMover][j].x-=10;
          }
          moverNave();
          if(hayDisparo){
            updateCannonball();
          }
          invaders[filaAMover][j].x_fin = invaders[filaAMover][j].x + 16;
          invaders[filaAMover][j].drawSpaceInvader(1, animacion);
        }
      }
    }
    bool puedeMoverAliens(){
      bool puedeMover = false;
      if(direccion){
        if (invaders[topeDerecha[1]][topeDerecha[0]].x_fin + 10 < tft.width()){
          puedeMover = true;
        }
        else{
          //direccion[filaAMover] = false;
          puedeMover = false;
        }
      }
      else {
        if (invaders[topeIzquierda[1]][topeIzquierda[0]].x > 0){
          puedeMover = true;
        }
        else{
          //direccion[filaAMover] = true;
          puedeMover = false;
        }
      }
      return puedeMover;
    }
    void updateLevel(){
      tft.setTextSize(1);
      tft.setCursor(120, 100);
      tft.print("Nivel: ");
      tft.setCursor(150, 100);
      tft.print(nivel, 10);
    }
};

Juego juego;
unsigned long tiempoT = millis();

void setup() {
  g_identifier = tft.readID();
  Serial.begin(9600);
  tft.begin(g_identifier);
  pinMode(xPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  tft.setRotation(0); // Ajusta la orientación de la pantalla según tu configuración
  tft.fillScreen(TFT_BLACK);
  juego = Juego();
  juego.reiniciar();
}



void loop() {
  while(!juego.get_gameOver()){
    juego.moverNave();
    juego.disparaNave();
    if(millis() - tiempoT >= tiempoDelay){
      tiempoT = millis();
      if(tiempoDelay > 10){
        tiempoDelay -= 10;
      }
      juego.updateAliens();
    } 
  }
  juego.gameOver();
}
