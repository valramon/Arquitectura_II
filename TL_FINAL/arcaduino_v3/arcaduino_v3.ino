#include <Arduino.h>
#include <MCUFRIEND_kbv.h> //libreria de la pantalla
#include <Adafruit_GFX.h>
#include "pitches.h"
#include "letrasMenu.h"
#include <EEPROM.h>
//#include <SPI.h>
/*
//pin definiciones
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
*/

/*
  ****************************************************************************************************************************************************
  ****************************************************************************************************************************************************
    BREAKOUT DEFINES
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define PRIMARY_COLOR 0x4A11
#define PRIMARY_LIGHT_COLOR 0x7A17
#define PRIMARY_DARK_COLOR 0x4016
#define PRIMARY_TEXT_COLOR 0x7FFF

#define LOWFLASH (defined(__AVR_ATmega328P__) && defined(MCUFRIEND_KBV_H_))

#define SCORE_SIZE 30

char *nick[3] = {"_","_","_"};

char scoreFormat[] = "%04d";

int highScore = 0;

typedef struct gameSize_type {
  int16_t x, y, width, height;
} gameSize_type;


gameSize_type gameSize;
uint16_t backgroundColor = BLACK;
int level;

const uint8_t BIT_MASK[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
uint8_t pointsForRow[] = {7, 7, 5, 5, 3, 3 , 1, 1};
#define GAMES_NUMBER 16

typedef struct game_type {
  int ballsize;
  int playerwidth;
  int playerheight;
  int exponent;
  int top;
  int rows;
  int columns;
  int brickGap;
  int lives;
  int wall[GAMES_NUMBER];
  int initVelx;
  int initVely;
} game_type;

game_type games[GAMES_NUMBER] =
  // ballsize, playerwidth, playerheight, exponent, top, rows, columns, brickGap, lives, wall[8],                                         initVelx, initVely
{
  { 10,             60,          8,           6,        40 ,     8,       8, 3,       3,  {0x18, 0x66, 0xFF, 0xDB, 0xFF, 0x7E, 0x24, 0x3C} , 28, -28},
  { 10,             50,          8,           6,        40 ,     8,       8, 3,       3,  {0xFF, 0x99, 0xFF, 0xE7, 0xBD, 0xDB, 0xE7, 0xFF} , 28, -28},
  { 10,             50,          8,           6,        40 ,     8,       8, 3,       3,  {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55} , 28, -28},
  { 8,              50,          8,           6,        40 ,     8,       8, 3,       3,  {0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF} , 34, -34},
  { 10,             40,          8,           6,        40 ,     8,       8, 3,       3,  {0xFF, 0xAA, 0xAA, 0xFF, 0xFF, 0xAA, 0xAA, 0xFF} , 28, -28},
  { 10,             40,          8,           6,        40 ,     8,       8, 3,       3,  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA} , 28, -28},
  { 12,             64,          8,           6,        60 ,     4,       2, 3,       4,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 20, -20},
  { 12,             60,          8,           6,        60 ,     5,       3, 3,       4,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 22, -22},
  { 10,             56,          8,           6,        30 ,     6,       4, 3,       4,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 24, -24},
  { 10,             52,          8,           6,        30 ,     7,       5, 3,       4,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 26, -26},
  { 8,              48,          8,           6,        30 ,     8,       6, 3,       3,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 28, -28},
  { 8,              44,          8,           6,        30 ,     8,       7, 3,       3,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 30, -30},
  { 8,              40,          8,           6,        30 ,     8,       8, 3,       3,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 32, -32},
  { 8,              36,          8,           6,        40 ,     8,       8, 3,       3,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 34, -34},
  { 8,              36,          8,           6,        40 ,     8,       8, 3,       3,  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA} , 34, -34}
};

game_type* gameBreakout;

typedef struct game_state_type {
  uint16_t ballx;
  uint16_t bally;
  uint16_t ballxold;
  uint16_t ballyold;
  int velx;
  int vely;
  int playerx;
  int playerxold;
  int wallState[8];
  int score;
  int remainingLives;
  int top;
  int bottom;
  int walltop;
  int wallbottom ;
  int brickheight;
  int brickwidth;


};
game_state_type state;

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    BREAKOUT DEFINES
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

// change this to whichever pin you want to use
int buzzer = 53;
MCUFRIEND_kbv tft;// = MCUFRIEND_kbv(LCD_CS, LCD_CD, LCD_WR, LCD_CS, LCD_RESET); //la pantallita sera tft
int16_t g_identifier;


//variables para el joystick
int xPin = A8; //EJE x
int yPin = A9; //EJE Y
int buttonPin = 39; //Apretar botón
int llamoMenu = 0;
char *pointer[] = { "Tetris", "Snake",  "Breakout", "Space Invaders"};
int cantidadJuegos = 4;

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    INICIO TETRIS
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/
#define PIECE_SIZE   10

//colores guardados
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0x778899
#define ORANGE  0xFF9900
#define PINK    0xFF66CC3
#define TURQUOISE 0x00e5ee


#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

// change this to make the song slower or faster
int tempo=160; 



// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  //Based on the arrangement at https://www.flutetunes.com/tunes.php?id=192
  
  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,

  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,
  

  NOTE_E5,2,  NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,2,   NOTE_A4,2,
  NOTE_GS4,2,  NOTE_B4,4,  REST,8, 
  NOTE_E5,2,   NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,4,   NOTE_E5,4,  NOTE_A5,2,
  NOTE_GS5,2,

};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes=sizeof(melody)/sizeof(melody[0])/2; 

// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

int game[32][18]; //matriz de piezas (paradas)

int posX, posY; //variables para saber pixel x e y
int posXBorrar = 150;
int posYBorrar = 0;
int fila,columna; //para saber fila y columna de la matriz que corresponde en la matriz
int primeraPieza;
int puntaje = 0;
int multiplicadorPorUno = 1;
int multiplicadorPorCinco = 5;
int multiplicadorPorDiez = 10;
int nivel = 0;

int xPosition = 0;
int yPosition = 0;
int buttonState = 1;

//pantalla inicial que muestra mensaje "Tetris"
void pantallaInicio() {
  tft.setCursor(90, 100); //nos ponemos en el pixel 50x150
  tft.setTextSize(4); //el texto sera de tamaño 4
  tft.setTextColor(GREEN);
  tft.println("Tetris!");
  tft.setCursor(80, 250); //nos ponemos en el pixel 50x150
  tft.setTextSize(2);
  while(digitalRead(buttonPin)){
    tft.setCursor(80, 250); //nos ponemos en el pixel 50x150
    tft.print("Touch to start");
    delay(500);
    tft.fillRect(80, 250, 200, 30, TFT_BLACK);
    delay(500);
    if(llamoMenu){
      return;
    }
  }
  //delay(2000);
  tft.fillScreen(BLACK); //pone pantalla en negro
}

//degradado de fonde de pantalla
void dibujoInicial() {
  int LCD_HEIGHT = 320;
  int LCD_WIDTH = 240;
  int c = LCD_HEIGHT / 29;
  for (int i = LCD_HEIGHT - 1; i >= 0; i -= 2)
  {
    tft.drawRect(0, i, LCD_WIDTH, 2, 0x07ff - i / c);
  }
}

//crear bordes con unos y matriz vacia con ceros
void inicicializarMatrizGame() {
  for (int fil = 0; fil < 32; fil++) {
    for (int col = 0; col < 18; col++) {
      if (fil == 31 || col == 0 || col == 17) {
        game[fil][col] = 1;
      } else {
        game[fil][col] = 0;
      }
    }
  }
}

//elimina el contenido de la parte de la pantalla en la que se mueven las piezas
void reiniciarPantalla() {
  // Dibuja un rectángulo de color CYAN en la parte superior
  //noTone(buzzer);
  tft.fillRect(70, 0, 160, 5, TFT_CYAN);

  // Dibuja un rectángulo de color BLACK en el resto de la pantalla
  tft.fillRect(70, 6, 160, 305, TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 350);
  tft.setTextSize(2);
  tft.print("Puntaje:");
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.fillRect(150, 350, 60, 60, TFT_BLACK);
  tft.setCursor(150,350),
  tft.print(puntaje,10);

  tft.setCursor(0, 380);
  tft.setTextSize(2);
  tft.print("Puntaje max:");
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.fillRect(180, 380, 60, 60, TFT_BLACK);
  tft.setCursor(180, 380),
  tft.print(highScore,10);
  //tone(buzzer, 700, 10);
}

//dibuja la parte estatica del Tetris
void dibujarPantalla() {
  // Cuadrado en donde se moverán las piezas
  tft.fillRect(70, 5, 160, 305, YELLOW);

  // Pantalla pequeña siguiente pieza
  tft.fillRect(5, 5, 51, 51, YELLOW);
}

//funcion que imprime una pieza --> LAS PIEZAS SON MATRICES DE 4*4
void draw(int pieza[4][4], int x, int y, int color) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (pieza[i][j]) { // Si en esa posición hay un 1
        tft.fillRect(x + j * PIECE_SIZE, y + i * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, color);
      }
    }
  }
}

//giro 90 grados de la pieza
void rotatePieza(int origen[4][4], int destino[4][4]) {
  for (int i = 0; i < 4; i++) {
    destino[0][i] = origen[i][2];
    destino[1][i] = origen[i][1];
    destino[2][i] = origen[i][0];
    destino[3][i] = origen[i][3];
  }
}

//funcion que mira si la pieza es un cuadrado (para no rotar la pieza)
int esCuadrado(int pieza[4][4]) {
  int es = 1;
  int cuadrado[4][4] = {
    {1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
  };
  for (int i = 0; i < 4 && es; i++) {
    for (int j = 0; j < 2 && es; j++) {
      if (pieza[i][j] != cuadrado[i][j]) {
        es = 0;
      }
    }
    tft.println();
  }
  return es;
}

//copia la pieza de origen a destino
void copiarPieza(int origen[4][4], int destino[4][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      destino[i][j] = origen[i][j];
    }
  }
}

//guarda la pieza (que ha caido) en la matriz game
void guardarPieza(int pieza[4][4], int fil, int col) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (pieza[i][j]) {
        game[fil + i][col + j] = 1;
      }
    }
  }
}

//[para depuracion] imprime la matriz de la pieza (para mostrar la matriz de ceros y unos)
void printPieza(int pieza[4][4]) {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      tft.print(pieza[i][j]);
    }
    tft.println("");
  }
}

//[para depuracion]pinta matriz de ceros y unos de game
void printGame() {
  tft.setCursor(0, 0);
  for (int fil = 0; fil < 32; fil++) {
    for (int col = 0; col < 18; col++) {
      tft.setTextSize(1);
      tft.print(game[fil][col]);
    }
    tft.println();
  }
}

//pinta las piezas que se hayan guardado en la matriz, es decir que hayan caido ya
void drawGame() {
  reiniciarPantalla();
  for (int i = 0; i < 32; i++) {
    for (int j = 1; j < 17; j++) {  // Comienza desde 1 y termina en 16 para evitar las paredes
      if (game[i][j]) {  // Si hay una pieza, píntala
        int x = 70 + (j - 1) * 10;
        int y = i * 10;
        tft.fillRect(x, y, 10, 10, MAGENTA);  // Dibuja un rectángulo de 10x10 en lugar de píxeles individuales
      }
    }
  }
  //delay(1000);
  //noTone(buzzer);
}

//funcion que calcula cuando una pieza toca el suelo/otra pieza de la matriz
int tocaSuelo(int pieza[4][4], int fil, int col, int n) {
  for (int i = 0; i < 4; i++) { //para ver las diferentes columnas
    if (pieza[n][i]) { //la pieza tiene un uno en esa columna
      if (game[fil + 1][col + i]) { //debajo hay un uno en la matriz
        return 1; //si toca
      }
    }
  }
  return 0;
}

//funcion que mueve la pieza una posicion a la derecha
void moverDerecha(int pieza[4][4], int fil, int col) {
    for (int i = 3; i >= 0; i--) {
      if (tocaDerecha(pieza, fil, col, i)) { //si toca no mover
        return;
      }
    }
    //si no toca a la derecha --> mover
    posXBorrar = posX;
    columna = col + 1;
    posX = posX + 10;
}

//ver si una pieza toca la derecha
int tocaDerecha(int pieza[4][4], int fil, int col, int n) {
  for (int i = 0; i < 4 ; i++) {
    if (pieza[n][i]) { //la pieza tiene un uno en esa fila
      if (game[fil + n][col + i + 1]) { //a la derecha hay un uno en la matriz
        return 1;
      }
    }
  }
  return 0;
}

//funcion que mueve la pieza una posicion a la izquierda
void moverIzquierda(int pieza[4][4], int fil, int col) {
    for (int i = 0; i < 3; i++) {
      if (tocaIzquierda(pieza, fil, col, i)) {
        return;
      }
    }
    //si no toca a la izq --> mover
    posXBorrar = posX;
    columna = col - 1;
    posX = posX - 10;
}

//ver si una pieza toca la izquierda
int tocaIzquierda(int pieza[4][4], int fil, int col, int n) {
  for (int i = 0; i < 4 ; i++) {
    if (pieza[n][i]) { //la pieza tiene un uno en esa fila
      if (game[fil + n][col + i - 1]) { //a la izquierda hay un uno en la matriz
        return 1;
      }
    }
  }
  return 0;
}

//funcion que comprueba si no se chocaria al rotar pieza (la rota pero no la cambia por si no pudiera)
int verSiPuedeRotar(int pieza[4][4], int fil, int col) {
  int npieza[4][4];
  copiarPieza(pieza, npieza);
  rotatePieza(npieza, npieza);
  for(int i = 0; i<4; i++) {
    if ((tocaDerecha(npieza, fil, col, i)) || (tocaIzquierda(npieza, fil, col, i)) || (tocaSuelo(npieza, fil, col, i))) {
      return 0; //no se podría rotar
    }
  }
  return 1;
}

//funcion que mira si hay alguna fila de unos en la matriz --> eliminar esa fila
void eliminarFilas() {
  int filasEliminadas = 0;
  for (int i = 1; i < 31; i++) {
    int cont = 0;
    int hayCero = 0;
    for (int j = 1; (j < 17 && (!hayCero)); j++) {
      if (game[i][j] == 0) {
        hayCero = 1;
      }
      else {
        cont++;
      }

    }
    if (cont == 16) {
      for (int k = i; k > 0; k--) {
        for (int l = 1; l < 17; l++) {
          game[k][l] = game[k - 1][l];
        }
      }
      filasEliminadas += 1;
      nivel += 1;
    }
  }
  if (filasEliminadas < 2){
    puntaje += (filasEliminadas * multiplicadorPorUno);
  }
  else if (filasEliminadas >= 2 && filasEliminadas < 5){
    puntaje += (filasEliminadas * multiplicadorPorCinco);
  }
  else {
    puntaje += (filasEliminadas * multiplicadorPorDiez);
  }
}

//si hay una pieza en la primera fila de la matriz --> se acaba el juego
int finPartida() {
  for(int i = 1; i < 17; i++) {
    if (game[1][i]) {
      return 1;
    }
  }
  return 0;
}

//Mensaje de finalizacion del juego
void mostrarMensajeFin() {
  tft.fillScreen(BLACK);
  tft.setCursor(10, 150); //nos ponemos en el pixel 50x150
  tft.setTextColor(RED);
  tft.setTextSize(3); //el texto sera de tamaño 4
  tft.println("Perdiste :(");
  while(digitalRead(buttonPin)){}
}

//funcion que muestra la siguiente pieza que entrara en el tetris
void mostrarPiezaDespues(int pieza[4][4], int color) {
  tft.fillRect(6, 6, 55, 55, TFT_BLACK);
  draw(pieza, 25, 18, color);
}

//funcion que inicia la bajada de piezas --> empieza el juego!
void inicioJuego() {
  //matriz de piezas del tetris
  int piezas[7][4][4] = {
    {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}
  };

  //matriz de colores de las piezas
  int colores[7] = {
      PINK,
      BLUE,
      GREEN,
      MAGENTA,
      YELLOW,
      ORANGE,
      TURQUOISE
  };

  //selecciono las piezas para que salgan
  int npieza[4][4];
  int pieza[4][4];
  unsigned long tiempoActual = millis();
  unsigned long tiempoBuzzer = millis();
  int randomPiezaDespues = random()%7;
  int randomPiezaAct;
  int thisNote = 0;
  int i = 0;
  //bucle que va sacando hasta 100 piezas para jugar
  while(!finPartida()){
     //elige una pieza al azar
    randomPiezaAct = randomPiezaDespues;
    randomPiezaDespues = (random()+3)%7;

    mostrarPiezaDespues(piezas[randomPiezaDespues], colores[randomPiezaDespues]);

    int cont = 1; //contador de movimiento de fila
    int toca = 0; //para saber si ha llegado abajo o toca alguna otra pieza
    int pieza[4][4];
    int npieza[4][4];
    int piezaBorrar[4][4];
    copiarPieza(piezas[randomPiezaAct],pieza);
    posX = 150; //pixel x del que sale
    columna = 9; //columna que corresponde a la matriz
    primeraPieza = 1;
    i = 5;
    //bucle que hace que piezas vayan bajando por la pantalla
    while((i<tft.height()-24) && (!toca)){
      //Serial.println(millis() - tiempoActual);
      if(millis() - tiempoActual >= 200){
        tiempoActual = millis();
      //for (int i = 5; (i<tft.height()-24) && (!toca); i+=10) {
        posY = i;
        if(!primeraPieza){
          draw(piezaBorrar, posXBorrar, posYBorrar, TFT_BLACK);
        }
        draw(pieza,posX,posY,colores[randomPiezaAct]); //pintar la pieza
        //lectura de x e y del joystick
        xPosition = analogRead(xPin);
        yPosition = analogRead(yPin);
        posXBorrar = posX;
        posYBorrar = i;
        //mover para abajo
        if (yPosition < 500) {
          //primero ver si tocaria el suelo al mover una posicion abajo
          for (int j = 3; (j>=0) && (!toca); j--) {
            if (tocaSuelo(pieza,cont+1,columna,j)) {
              toca = 1; //si toca suelo-->se para
              guardarPieza(pieza,cont-j+1,columna);
              //pintar la matriz de las piezas por pantalla
              drawGame();
            }
          }
          if (!toca) { //si no toca se puede bajar
            cont++;
            posYBorrar = i;
            i = i + 10;
            fila++;
          }
          //
        }
        else{
            //mirar si toca el suelo/otra pieza
          for (int j = 3; (j>=0) && (!toca); j--) {
            if (tocaSuelo(pieza,cont,columna,j)) {
              toca = 1; //si toca suelo-->se para
              guardarPieza(pieza,cont-j,columna);
              //pintar la matriz de las piezas por pantalla
              drawGame();
            }
          }

        }
        copiarPieza(pieza, piezaBorrar);
        //si pasa ciertos valores --> mover a izq/dcha o abajo
        if (xPosition < 500) {
          moverIzquierda(pieza, cont, columna);
        } else  if (xPosition > 600) {
          moverDerecha(pieza, cont, columna);
        }
        //ver si hemos pulsado el boton de rotar la pieza
        buttonState = digitalRead(buttonPin);
        if (!buttonState && !esCuadrado(pieza)) {
          if (verSiPuedeRotar(pieza, fila, columna)) {
            copiarPieza(pieza, piezaBorrar);
            rotatePieza(pieza, npieza);
            copiarPieza(npieza, pieza);
            buttonState = 1;
          }
        }
        primeraPieza = 0;
        //volvemos a mirar la rotacion
        cont++; //actualizar contador de filas
        i += 10;
      // stop the waveform generation before the next note.
      }
      if(millis() - tiempoBuzzer >= noteDuration){
        // calculates the duration of each note
        divider = melody[thisNote + 1];
        if (divider > 0) {
          // regular note, just proceed
          noteDuration = (wholenote) / divider;
        } else if (divider < 0) {
          // dotted notes are represented with negative durations!!
          noteDuration = (wholenote) / abs(divider);
          noteDuration *= 1.5; // increases the duration in half for dotted notes
        }
        tiempoBuzzer = millis();
        // we only play the note for 90% of the duration, leaving 10% as a pause
        tone(buzzer, melody[thisNote], noteDuration*0.9);
        thisNote = thisNote + 2;
        if (thisNote >= notes * 2){
          thisNote = 0;
        }
      }
      //delay(100);
      
      eliminarFilas(); //por si hubiera una fila completa --> eliminarla
      //por si se hubiera acabado el juego
      if (finPartida()) {
        if(puntaje > highScore){
          EEPROM.update(0, puntaje);
          highScore = puntaje;
        }
        
        mostrarMensajeFin();
        //tetris();
        return;
      }
      if(llamoMenu){
        return;
      }
    }
  }
}

void tetris (){
  do{
    tft.fillScreen(BLACK); //pone pantalla en negro
    EEPROM.get(0, highScore);
    pantallaInicio();
    //dibujarPantalla();
    inicicializarMatrizGame();
    dibujoInicial();
    reiniciarPantalla();
    inicioJuego();
  }while(!llamoMenu);
  llamoMenu = 0;
  tft.fillScreen(TFT_BLACK);
  printTitulo();
  drawMenu();
}

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    FIN TETRIS
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    INICIO SNAKE
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5

//int button=6;
uint16_t pixelColor;
uint8_t buttonPressed, currentDirection;
  byte newPos[2];
  byte oldPos[2];
int ancho = 479;
int altura = 319;
boolean collision, appleOn, displayEnd, specialOn, allowSpecial, showTitle = true;
int head, tiempo, snakeSize, pixelLocationXAxis, pixelLocationYAxis, m[300], n[300],
    appleX, appleY, yMultiplier, selection = 100, difficulty, specialX, specialY, specialTime;


uint8_t readButton(void) {
  if (analogRead(yPin)<400) return BUTTON_DOWN;
  if (analogRead(xPin)>700) return BUTTON_RIGHT;
  if (!digitalRead(buttonPin)) return BUTTON_SELECT;
  if (analogRead(yPin)>700) return BUTTON_UP;
  if (analogRead(xPin)<400) return BUTTON_LEFT;
  else return BUTTON_NONE;
}

void showTitlScreen(){
  tft.fillRect(0,0,480,320, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_RED);
  tft.setCursor(5,30);
  tft.print("SNAKE");
  titleSelection();
}
void titleSelection(){
  uint8_t titleButton = readButton();
  delay(200);
  if (titleButton == BUTTON_UP)
    selection--;
    //drawBoard();
  if (titleButton == BUTTON_DOWN)
    selection++;
    
 if (selection % 3 == 0){ // easy mode
    tft.setTextColor(TFT_YELLOW);
    //tft.fillRect(25, 75, 50, 18);
    tft.setCursor(60, 80);
    tft.print("Easy");
    tft.setTextColor(TFT_WHITE);
    //tft.fillRect(25, 95, 50, 18);
    tft.setCursor(60, 100);
    tft.print("Medium");
    tft.setTextColor(TFT_WHITE);
    //tft.fillRect(25, 115, 50, 18);
    tft.setCursor(60, 120);
    tft.print("Hard");
    difficulty = 60;
  } else if (selection % 3 == 1){ // normal mode
    tft.setTextColor(TFT_WHITE);
    //tft.fillRect(25, 75, 50, 18);
    tft.setCursor(60, 80);
    tft.print("Easy");
    tft.setTextColor(TFT_YELLOW);
    //tft.fillRect(25, 95, 50, 18);
    tft.setCursor(60, 100);
    tft.print("Medium");
    tft.setTextColor(TFT_WHITE);
    //tft.fillRect(25, 115, 50, 18);
    tft.setCursor(60, 120);
    tft.print("Hard");
    difficulty = 40;
  } else { // hard mode
    tft.setTextColor(TFT_WHITE);
    //tft.fillRect(25, 75, 50, 18);
    tft.setCursor(60, 80);
    tft.print("Easy");
    tft.setTextColor(TFT_WHITE);
    //tft.fillRect(25, 95, 50, 18);
    tft.setCursor(60, 100);
    tft.print("Medium");
    tft.setTextColor(TFT_YELLOW);
    //tft.fillRect(25, 115, 50, 18);
    tft.setCursor(60, 120);
    tft.print("Hard");
    difficulty = 20;
  }
  
  if(titleButton == BUTTON_SELECT){
    showTitle = false;
    //drawBoard();
    tft.fillScreen(TFT_BLACK); 
    return;
  }
  if(llamoMenu){
    return;
  }
  
  delay(100);
  titleSelection();
}

void drawBoard(){
  tft.drawRect(0, 9, ancho, altura - 9, (255,0,255));
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(3, 0);
  tft.print("Score:");
  //drawApple(appleX,appleY,(0,255,0));
}

void updateScore(int puntaje){
  tft.drawRect(38, 0, ancho - 75, 9, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(50,0),
  tft.print(puntaje,10);
}

void updateSnakePosition(uint8_t buttonPressed){

  if (buttonPressed == BUTTON_UP){
    if(currentDirection != BUTTON_DOWN){
      pixelLocationYAxis -= 3;
      currentDirection = BUTTON_UP;
    } else
      pixelLocationYAxis += 3;
  }
  if (buttonPressed == BUTTON_DOWN){
    if(currentDirection != BUTTON_UP){
      pixelLocationYAxis += 3;
      currentDirection = BUTTON_DOWN;
    } else
      pixelLocationYAxis -= 3;
  }  
  if (buttonPressed == BUTTON_LEFT){
    if(currentDirection != BUTTON_RIGHT){
      pixelLocationXAxis -= 3;
      currentDirection = BUTTON_LEFT;
    } else
      pixelLocationXAxis += 3;
  }  
  if (buttonPressed == BUTTON_RIGHT){
    if(currentDirection != BUTTON_LEFT){
      pixelLocationXAxis += 3;
      currentDirection = BUTTON_RIGHT;
    } else
      pixelLocationXAxis -= 3;
  }
    
  if (pixelLocationYAxis < 10)
    pixelLocationYAxis = 10;
  if (pixelLocationYAxis > altura - 3)
    pixelLocationYAxis = altura - 3;
  if (pixelLocationXAxis < 1)
    pixelLocationXAxis = 1;
  if (pixelLocationXAxis > ancho - 3)
    pixelLocationXAxis = ancho - 3;
    
  drawSnake(pixelLocationXAxis,pixelLocationYAxis, TFT_WHITE);
  m[head] = pixelLocationXAxis;
  n[head] = pixelLocationYAxis;
  collisionCheck();
  head++;
  if (head >= 300){
    removeLastFromTail();
    resetArray();
    head = snakeSize + 1;
  }
}

void resetArray(){
  for(int j = 1; j < snakeSize; j++){
    m[snakeSize - j] = m[head - j];
    m[head - j] = 0;
    n[snakeSize - j] = n[head - j];
    n[head - j] = 0;
  }
  m[0] = 0;
  n[0] = 0;
}

void drawSnake(int pixelLocationXAxis,int pixelLocationYAxis,uint16_t pixelColor){
  /*tft.setColorTFT_BLACK;
  tft.fillRect(oldPos[0]*10,oldPos[1]*10,10,10);
  tft.setColor(pixelColor);
  tft.fillRect(newPos[0]*10,newPos[1]*10,10,10);*/
  /*tft.setColorTFT_WHITE;
  tft.fillRect(40,30,46,32);*/
  tft.drawPixel(pixelLocationXAxis - 2, pixelLocationYAxis - 2, pixelColor);  //top left

  tft.drawPixel(pixelLocationXAxis, pixelLocationYAxis - 2, pixelColor);      //top

  tft.drawPixel(pixelLocationXAxis + 2, pixelLocationYAxis - 2, pixelColor);  //top right

  tft.drawPixel(pixelLocationXAxis - 2, pixelLocationYAxis, pixelColor);      //left

  tft.drawPixel(pixelLocationXAxis, pixelLocationYAxis, pixelColor);          //middle

  tft.drawPixel(pixelLocationXAxis + 2, pixelLocationYAxis, pixelColor);      //right

  tft.drawPixel(pixelLocationXAxis - 2, pixelLocationYAxis + 2, pixelColor);  //bottom left

  tft.drawPixel(pixelLocationXAxis, pixelLocationYAxis + 2, pixelColor);      //bottom

  tft.drawPixel(pixelLocationXAxis + 2, pixelLocationYAxis + 2, pixelColor);  //bottom right
}

void removeLastFromTail(){
  drawSnake(m[head - snakeSize], n[head - snakeSize],TFT_BLACK);
  m[head - snakeSize] = 0;
  n[head - snakeSize] = 0;
}

void collisionCheck(){
  if (m[head] < 2 || m[head] > ancho - 2 || n[head] <= 11 || n[head] >= altura){
    pixelColor = 255,0,0;
    collision = true;
    drawSnake(pixelLocationXAxis, pixelLocationYAxis, pixelColor);
    displayEnd = true;
  }
  
  for (int z = 0; z < head; z++) {
    if ((m[head] == m[z]) && (n[head] == n[z])) {
      pixelColor = 255,0,0;
      collision = true;
      drawSnake(pixelLocationXAxis, pixelLocationYAxis,pixelColor);
      displayEnd = true;
    }
  }
}

void appleLogic(){
  if (!appleOn){
    randomSeed(millis());
    appleX = random(2, ancho - 5);
    Serial.println(appleX);
    appleY = 10;
    yMultiplier = random(1, 49);
    if (appleX % 3 == 1)
      appleX--;
    else if (appleX % 3 == 2)
      appleX++;
    appleY += (3 * yMultiplier);
    drawApple(appleX, appleY);
    appleOn = true;
  }
}

void checkIfAppleGot(){
  if ((m[head - 2] == appleX)||(m[head - 2] == appleX - 1)||(m[head - 2] == appleX + 1)){
    if ((n[head - 2] == appleY)||(n[head - 2] == appleY - 1)||(n[head - 2] == appleY + 1)){
      appleOn = false;
      deleteApple(appleX, appleY);
      snakeSize += 5;
      puntaje += 10;
      updateScore(puntaje);
      allowSpecial = true;
    }
  } else
    drawApple(appleX, appleY);
}

void specialApple(){
  if (!specialOn && (puntaje % 100 == 0) && allowSpecial){
    randomSeed(millis());
    specialX = random(2, ancho - 5);
    specialY = 10;
    yMultiplier = random(1, 49);
    if (specialX % 3 == 1)
      specialX--;
    else if (specialX % 3 == 2)
      specialX++;
    specialY += (3 * yMultiplier);
    deleteApple(appleX, appleY);
    drawAppleSpecial(specialX, specialY);
    specialTime = 0;
    specialOn = true;
    allowSpecial = false;
  }
}

void checkIfSpecialGot(){
  if (specialOn){
    if ((m[head - 1] == specialX)||(m[head - 1] == specialX-1)||(m[head - 1] == specialX+1)){
      if ((n[head - 1] == specialY)||(n[head - 1] == specialY-1)||(n[head - 1] == specialY+1)){
        specialSelection();
        deleteSpecial(specialX, specialY);
      }
    } else{
      drawAppleSpecial(specialX, specialY);
    }
  }
}

void specialSelection(){
  int removeSize = random(1, 5);
  int addedScore = random(1, 11) * 100;
  snakeSize -= removeSize;
  for (int i = 0; i < removeSize; i++){
    drawSnake(m[head - snakeSize - i], n[head - snakeSize - i], TFT_BLACK);
    m[head - snakeSize - i] = 0;
    n[head - snakeSize - i] = 0;
  }
  puntaje += addedScore;
  updateScore(puntaje);
  specialOn = false;
}

void specialTimer(){
  specialTime++;
  if (specialTime > 150){
    drawApple(specialX, specialY);
    specialOn = false;
    allowSpecial == false;
    specialTime = 0;
  }
}

void drawApple(int appleX, int appleY){

  tft.drawPixel(appleX - 1, appleY - 1, TFT_RED);  //top left

  tft.drawPixel(appleX, appleY - 1, TFT_RED);      //top

  tft.drawPixel(appleX + 1, appleY - 1, TFT_RED);  //top right

  tft.drawPixel(appleX - 1, appleY, TFT_RED);      //left

  tft.drawPixel(appleX, appleY, TFT_RED);          //middle

  tft.drawPixel(appleX + 1, appleY, TFT_RED);      //right

  tft.drawPixel(appleX - 1, appleY + 1, TFT_RED);  //bottom left

  tft.drawPixel(appleX, appleY + 1, TFT_RED);      //bottom

  tft.drawPixel(appleX + 1, appleY + 1, TFT_RED);  //bottom right
}
void deleteApple(int appleX, int appleY){

  tft.drawPixel(appleX - 3, appleY - 3, TFT_BLACK);  //top left

  tft.drawPixel(appleX, appleY - 3, TFT_BLACK);      //top

  tft.drawPixel(appleX + 3, appleY - 3, TFT_BLACK);  //top right

  tft.drawPixel(appleX - 3, appleY, TFT_BLACK);      //left

  tft.drawPixel(appleX, appleY, TFT_BLACK);          //middle
 
  tft.drawPixel(appleX + 3, appleY, TFT_BLACK);      //right

  tft.drawPixel(appleX - 3, appleY + 3, TFT_BLACK);  //bottom left

  tft.drawPixel(appleX, appleY + 3, TFT_BLACK);      //bottom

  tft.drawPixel(appleX + 3, appleY + 3, TFT_BLACK);      //bottom
}
void drawAppleSpecial(int specialX, int specialY){

  tft.drawPixel(specialX - 1, specialY - 1, TFT_YELLOW);  //top left

  tft.drawPixel(specialX, specialY - 1, TFT_YELLOW);      //top

  tft.drawPixel(specialX + 1, specialY - 1, TFT_YELLOW);  //top right

  tft.drawPixel(specialX - 1, specialY, TFT_YELLOW);      //left

  tft.drawPixel(specialX, specialY, TFT_YELLOW);          //middle

  tft.drawPixel(specialX + 1, specialY, TFT_YELLOW);      //right

  tft.drawPixel(specialX - 1, specialY + 1, TFT_YELLOW);  //bottom left

  tft.drawPixel(specialX, specialY + 1, TFT_YELLOW);      //bottom

  tft.drawPixel(specialX + 1, specialY + 1, TFT_YELLOW);  //bottom right
}
void deleteSpecial(int specialX, int specialY){

  tft.drawPixel(specialX - 2, specialY - 2, TFT_BLACK);  //top left

  tft.drawPixel(specialX, specialY - 2, TFT_BLACK);      //top

  tft.drawPixel(specialX + 2, specialY - 2, TFT_BLACK);  //top right

  tft.drawPixel(specialX - 2, specialY, TFT_BLACK);      //left

  tft.drawPixel(specialX, specialY, TFT_BLACK);          //middle

  tft.drawPixel(specialX + 2, specialY, TFT_BLACK);      //right

  tft.drawPixel(specialX - 2, specialY + 2, TFT_BLACK);  //bottom left

  tft.drawPixel(specialX, specialY + 2, TFT_BLACK);      //bottom

  tft.drawPixel(specialX + 2, specialY + 2, TFT_BLACK);  //bottom right
}
void displayEndingScreen(){
  tft.drawRect(25, 65, ancho - 50, 30, TFT_CYAN);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(35,70);
  tft.print(puntaje,10);
  tft.setCursor(35,120);
  tft.print(highScore,10);
  displayEnd = false;
}
void inicioSnake(){
    //tft.InitLCD();
  tft.fillScreen(TFT_BLACK);
  if (showTitle && !llamoMenu)
    showTitlScreen();
  //tft.fillScreen(0x0000);
  collision = false;
  appleOn = false;
  specialOn = false;
  allowSpecial = false;
  displayEnd = false;
  pixelLocationXAxis = ancho / 2 - 1;
  pixelLocationYAxis = altura / 2 - 1;
  pixelColor = TFT_WHITE;
  buttonPressed = BUTTON_RIGHT;
  currentDirection = BUTTON_RIGHT;
  specialTime = 0;
  puntaje = 0;
  head = 0;
  snakeSize = 10;
  tiempo = 0;
  updateScore(puntaje);
  drawBoard();
}

void snake(){
  inicioSnake();
  EEPROM.get(200, highScore);
  do{
    uint8_t b = readButton();
    if (b != BUTTON_NONE && b != BUTTON_SELECT) buttonPressed = b;
    if (!collision){
      appleLogic();
      checkIfAppleGot();
      specialApple();
      checkIfSpecialGot();
      specialTimer();
      updateSnakePosition(buttonPressed);
      if (tiempo >= snakeSize - 1)
        removeLastFromTail();
      else
        tiempo++;
    } else {
      if (displayEnd == true)
        if(puntaje > highScore){
          EEPROM.update(200, puntaje);
          highScore = puntaje;
        }
        displayEndingScreen();
        showTitle = true;
        uint8_t buttonPressed = readButton();
        delay(200);
        if (buttonPressed == BUTTON_SELECT){
          inicioSnake();
    }
          
    }
    delay(difficulty);
  }while(!llamoMenu);
  llamoMenu = 0;
  showTitle = true;
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  printTitulo();
  drawMenu();
}

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    FIN SNAKE
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    INICIO BREAKOUT
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/


void newGame(game_type *newGame, game_state_type * state,  MCUFRIEND_kbv &tft) {
  gameBreakout = newGame;
  setupState(gameBreakout, state, tft);

  clearDialog(gameSize);
  updateLives(gameBreakout->lives, state->remainingLives);
  updateScoreBreakout(state->score);

  setupWall(gameBreakout, state);

  touchToStart();

  clearDialog(gameSize);
  updateLives(gameBreakout->lives, state->remainingLives);
  updateScoreBreakout(state->score);
  setupWall(gameBreakout, state);
}

void setupStateSizes(game_type* game, game_state_type * state, MCUFRIEND_kbv &tft) {
  state->bottom = tft.height() - 30;
  state->brickwidth = tft.width() / game->columns;
  state->brickheight = tft.height() / 24;
}

void setupState(game_type* game, game_state_type * state, MCUFRIEND_kbv &tft) {
  setupStateSizes(game, state, tft);
  for (int i = 0; i < game->rows ; i ++) {
    state->wallState[i] = 0;
  }
  state->playerx = tft.width() / 2 - game->playerwidth / 2;
  state->remainingLives = game->lives;
  state->bally = state->bottom << game->exponent;
  state->ballyold = state->bottom << game->exponent;
  state->velx = game->initVelx;
  state->vely = game->initVely;
}

void updateLives(int lives, int remainingLives) {

  for (int i = 0; i < lives; i++) {
    tft.fillCircle((1 + i) * 15, 15, 5, BLACK);
  }

  for (int i = 0; i < remainingLives; i++) {
    tft.fillCircle((1 + i) * 15, 15, 5, YELLOW);
  }

}


void setupWall(game_type * game, game_state_type * state) {

  int colors[] = {RED, RED, BLUE, BLUE,  YELLOW, YELLOW, GREEN, GREEN};
  state->walltop = game->top + 40;
  state->wallbottom = state->walltop + game->rows * state->brickheight;
  for (int i = 0; i < game->rows; i++) {
    for (int j = 0; j < game->columns; j++) {
      if (isBrickIn(game->wall, j, i)) {
        setBrick(state->wallState, j, i);
        drawBrick(state, j, i, colors[i]);
      }
    }
  }
}


void drawBrick(game_state_type * state, int xBrick, int yBrickRow, uint16_t backgroundColor) {
  tft.fillRect((state->brickwidth * xBrick) + gameBreakout->brickGap,
               state->walltop + (state->brickheight * yBrickRow) + gameBreakout->brickGap ,
               state->brickwidth - gameBreakout->brickGap * 2,
               state->brickheight -  gameBreakout->brickGap * 2, backgroundColor);

}


boolean noBricks(game_type * game, game_state_type * state) {
  for (int i = 0; i < game->rows ; i++) {
    if (state->wallState[i]) return false;
  }
  return true;
}

void drawPlayer(game_type * game, game_state_type * state) {
  // paint
  tft.fillRect(state->playerx, state->bottom, game->playerwidth, game->playerheight, YELLOW);
  if (state->playerx != state->playerxold) {
    // remove old pixels
    if (state->playerx < state->playerxold) {
      tft.fillRect(state->playerx + game->playerwidth, state->bottom, abs(state->playerx - state->playerxold), game->playerheight, backgroundColor);
    }
    else {
      tft.fillRect(state->playerxold, state->bottom, abs(state->playerx - state->playerxold), game->playerheight, backgroundColor);
    }

  }
}

void drawBall(int x, int y, int xold, int yold, int ballsize) {
  // remove old pixels
  //if (xold != x && yold != y) {
  if (xold <= x && yold <= y) {
    tft.fillRect(xold , yold, ballsize, y - yold, BLACK);
    tft.fillRect(xold , yold, x - xold, ballsize, BLACK);
  } else if (xold >= x && yold >= y) {
    tft.fillRect(x + ballsize , yold, xold - x, ballsize, BLACK);
    tft.fillRect(xold , y + ballsize, ballsize, yold - y, BLACK);
  } else if (xold <= x && yold >= y) {
    tft.fillRect(xold , yold, x - xold, ballsize, BLACK);
    tft.fillRect(xold , y + ballsize, ballsize, yold - y, BLACK);
  } else if (xold >= x && yold <= y) {
    tft.fillRect(xold , yold, ballsize, y - yold, BLACK);
    tft.fillRect(x + ballsize, yold, xold - x, ballsize, BLACK);
  }
  // paint new ball
  tft.fillRect(x , y, ballsize, ballsize, YELLOW);
  // }

}

void touchToStart() {
  drawBoxedString(0, 200, "   BREAKOUT", 3, YELLOW, BLACK);
  drawBoxedString(0, 240, "    TOUCH TO START", 2, RED, BLACK);
  while (waitForTouch() < 0) {}
}

void gameOverTouchToStart() {
  drawBoxedString(0, 180, "  GAME OVER", 3, YELLOW, BLACK);
  drawBoxedString(0, 220, "  TOUCH TO START", 2, RED, BLACK);
  tft.setCursor(0, 250);
  tft.print("Puntaje max: ");
  tft.setCursor(180, 250);
  tft.print(highScore, 10);
  while (waitForTouch() < 0) {}
}


void updateScoreBreakout (int score) {
  char buffer[5];
  snprintf(buffer, sizeof(buffer), scoreFormat, score);
  drawBoxedString(tft.width() - 50, 6, buffer, 2, YELLOW, PRIMARY_DARK_COLOR);
}

void checkBrickCollision(game_type* game, game_state_type * state, uint16_t x, uint16_t y) {
  int x1 = x + game->ballsize;
  int y1 = y + game->ballsize;
  int collissions = 0;
  collissions += checkCornerCollision(game, state, x, y);
  collissions += checkCornerCollision(game, state, x1, y1);
  collissions += checkCornerCollision(game, state, x, y1);
  collissions += checkCornerCollision(game, state, x1, y);
  if (collissions > 0 ) {
    tone(buzzer, NOTE_A3, 4);
    state->vely = (-1 * state->vely);
    if ((((x % state->brickwidth) == 0)  && ( state->velx < 0 ))
        || ((((x + game->ballsize) % state->brickwidth) == 0)  && ( state->velx > 0 )) ) {
      state->velx = (-1 * state->velx);
    }
  }


}
int checkCornerCollision(game_type * game,  game_state_type * state, uint16_t x, uint16_t y) {
  if ((y > state->walltop) && (y < state->wallbottom)) {
    int yBrickRow = ( y -  state->walltop) / state->brickheight;
    int xBrickColumn = (x / state->brickwidth);
    if (isBrickIn(state->wallState, xBrickColumn, yBrickRow) ) {
      hitBrick(state, xBrickColumn, yBrickRow);
      return 1;
    }
  }
  return 0;
}
void hitBrick(game_state_type * state, int xBrick, int yBrickRow) {
  state->score += pointsForRow[yBrickRow];
  drawBrick(state, xBrick, yBrickRow, WHITE);
  delay(16);
  drawBrick(state, xBrick, yBrickRow, BLUE);
  delay(8);
  drawBrick(state, xBrick, yBrickRow, backgroundColor);
  unsetBrick(state->wallState, xBrick, yBrickRow);
  updateScoreBreakout(state->score);
}

void checkBorderCollision(game_type * game,  game_state_type * state, uint16_t x, uint16_t y) {
  // check wall collision
  if (x + game->ballsize >=  tft.width()) {
    state->velx = -abs(state->velx);
    tone(buzzer, NOTE_D4, 16);
  }
  if (x <= 0  ) {
    state->velx = abs(state->velx);
    tone(buzzer, NOTE_D4, 16);
  }
  if (y <= SCORE_SIZE ) {
    state->vely = abs(state->vely);
    tone(buzzer, NOTE_D4, 16);
  }
  if (((y + game->ballsize)  >=  state->bottom)
      && ((y + game->ballsize) <= (state->bottom + game->playerheight))
      && (x >= state->playerx)
      && (x <= (state->playerx + game->playerwidth))) {
    // change vel x near player borders
    if (x > (state->playerx + game->playerwidth - 6)) {
      state->velx = state->velx - 1;
    } else if (x < state->playerx + 6) {
      state->velx = state->velx + 1;
    }
    state->vely = -abs(state->vely) ;
    tone(buzzer, NOTE_B5, 16);
  }
}

void checkBallCollisions(game_type * game, game_state_type * state, uint16_t x, uint16_t y) {
  checkBrickCollision(game, state, x, y);
  checkBorderCollision(game, state, x, y);
}

void checkBallExit(game_type * game, game_state_type * state, uint16_t x, uint16_t y) {
  if (((y + game->ballsize)  >=  tft.height())) {
    state->remainingLives--;
    updateLives(game->lives, state->remainingLives);
    delay(500);
    state->vely = -abs(state->vely) ;
  }
}

void setBrick(int wall[], uint8_t x, uint8_t y) {
  wall[y] = wall[y] | BIT_MASK[x];
}

void unsetBrick(int wall[], uint8_t x, uint8_t y) {
  wall[y] = wall[y] & ~BIT_MASK[x];
}

boolean isBrickIn(int wall[], uint8_t x, uint8_t y) {
  return wall[y] &  BIT_MASK[x];
}

//////////////////////////////////////////////////////////////
// Screen Painting methods
//////////////////////////////////////////////////////////////


/**
  Print a text in forecolor over a filled box with background color.
  Rectangle size is calculated to include the whole text without margins

  @param x horizontal coordinate in points left upper corner
  @param y vertical coordinate in points left upper corner
  @param fontsize font size of the text to print
  @param foreColor forecolor of the text to print
  @param backgroundColor color of the filled rect
  @return void
*/
void drawBoxedString(const uint16_t x, const uint16_t y, const char* string, const uint16_t fontsize, const uint16_t foreColor, const uint16_t backgroundColor) {
  tft.setTextSize(fontsize);
  int16_t  x1, y1;
  uint16_t w, h;
  tft.getTextBounds(string, x, y, &x1, &y1, &w, &h);
  tft.fillRect(x, y, w, h, backgroundColor);
  tft.setCursor(x, y);
  tft.setTextColor(foreColor);
  tft.print(string);
}

/**
  Clear the screen to the default backgrounds

  @param void
  @return void
*/
void clearDialog(gameSize_type gameSize) {
  tft.fillRect(gameSize.x, gameSize.y, gameSize.width, gameSize.height,  backgroundColor);
  tft.fillRect(gameSize.x, gameSize.y, gameSize.width, SCORE_SIZE, PRIMARY_DARK_COLOR);
}



//////////////////////////////////////////////////////////////
// READ UI SELECTION
//////////////////////////////////////////////////////////////

/*
   Checks if the user is selecting any of the visible enabled ui elements
   The onTap callback of the selected element is called and it set as pressed

  @param lastSelected the last selection
  @return the new selection

*/
int readUiSelection(game_type * game, game_state_type * state, const int16_t lastSelected ) {
  int16_t xpos = analogRead(xPin);  //screen coordinates
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  if (xpos > 600 || xpos < 480) {
    if (xpos > 600) {
      state->playerx += 2;
    } else {
      state->playerx -= 2;
    }
    if (state->playerx >= tft.width() - game->playerwidth) state->playerx = tft.width() - game->playerwidth;
    if (state->playerx < 0) state->playerx = 0;
    return 1;

  }
#ifdef DEMO_MODE
  state->playerx = (state->ballx >> game->exponent) - game->playerwidth / 2;
  if (state->playerx >= tft.width() - game->playerwidth) state->playerx = tft.width() - game->playerwidth;
  if (state->playerx < 0) state->playerx = 0;
#endif

  return -1;
}


int waitForTouch() {
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (!digitalRead(buttonPin)) {
    return 1;
  }
  return -1;
}


int selectionBreakout = -1;

void breakout(){
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  gameSize = {0, 0, tft.width(), tft.height()};
  state.score = 0;
  EEPROM.get(100, highScore);
  newGame(&games[0], &state, tft);
  do{
    selectionBreakout = readUiSelection(gameBreakout, &state, selectionBreakout);

    drawPlayer(gameBreakout, &state);
    // store old position to remove old pixels
    state.playerxold = state.playerx;

    // calculate new ball position x1 = x0 + vx * dt

    // check max speed
    if (abs( state.vely) > ((1 << gameBreakout->exponent) - 1)) {
      state.vely = ((1 << gameBreakout->exponent) - 1) * ((state.vely > 0) - (state.vely < 0));
    }
    if (abs( state.velx) > ((1 << gameBreakout->exponent) - 1)) {
      state.velx = ((1 << gameBreakout->exponent) - 1) * ((state.velx > 0) - (state.velx < 0));
    }

    state.ballx += state.velx;
    state.bally += state.vely;

    // check ball collisions and exit
    checkBallCollisions(gameBreakout, &state, state.ballx >> gameBreakout->exponent, state.bally >> gameBreakout->exponent);
    checkBallExit(gameBreakout, &state, state.ballx >> gameBreakout->exponent, state.bally >> gameBreakout->exponent);

    // draw ball in new position
    drawBall(state.ballx >> gameBreakout->exponent, state.bally >> gameBreakout->exponent, state.ballxold >> gameBreakout->exponent, state.ballyold >> gameBreakout->exponent, gameBreakout->ballsize );

    // store old position to remove old pixels
    state.ballxold = state.ballx;
    state.ballyold = state.bally;

    // increment velocity
    state.velx = (20 + (state.score >> 3 )) * ( (state.velx > 0) - (state.velx < 0));
    state.vely = (20 + (state.score >> 3 )) * ( (state.vely > 0) - (state.vely < 0));

    // if no bricks go to next level
    if (noBricks(gameBreakout, &state) && level < GAMES_NUMBER) {
      level++;
      newGame( &games[level], &state, tft);
    } else if ( state.remainingLives <= 0) {
      if(state.score > highScore){
        EEPROM.update(100, state.score);
        highScore = state.score;
      }
      gameOverTouchToStart();
      state.score = 0;
      //level = 0;
      newGame(gameBreakout, &state, tft);
    }
  }while(!llamoMenu);
  llamoMenu = 0;
  showTitle = true;
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  printTitulo();
  drawMenu();
}

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    FIN BREAKOUT
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    INICIO SPACE INVADERS
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

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
      tft.setCursor(80, 50);
      tft.setTextSize(2);
      tft.setTextColor(TFT_DARKGREEN);
      tft.print("Space Invaders");
      tft.setTextSize(1);
      tft.drawBitmap(80, 150, alienFilaUno, 16, 16, coloresAliens[0]);
      tft.setCursor(130, 155);
      tft.print(50, 10);
      tft.drawBitmap(80, 180, alienFilaDos, 16, 16, coloresAliens[1]);
      tft.setCursor(130, 185);
      tft.print(40, 10);
      tft.drawBitmap(80, 210, alienFilaTres, 16, 16, coloresAliens[2]);
      tft.setCursor(130, 215);
      tft.print(30, 10);
      tft.drawBitmap(80, 240, alienFilaCuatro, 16, 16, coloresAliens[3]);
      tft.setCursor(130, 245);
      tft.print(20, 10);
      tft.drawBitmap(80, 270, alienFilaCinco, 16, 16, coloresAliens[4]);
      tft.setCursor(130, 275);
      tft.print(10, 10);
      while(digitalRead(buttonPin)){
        tft.setCursor(120, 350); //nos ponemos en el pixel 50x150
        tft.print("Touch to start");
        delay(500);
        tft.fillRect(120, 350, 200, 30, TFT_BLACK);
        delay(500);
        if(llamoMenu){
          return;
        }
      }
      tft.fillScreen(TFT_BLACK);
    }
    void gameOver(){
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(120, 100);
      tft.setTextColor(TFT_DARKGREEN);
      tft.print("GAME OVER!");
      if(puntaje > highScore){
        EEPROM.update(300, puntaje);
        highScore = puntaje;
      }
      tft.setCursor(120, 300);
      tft.print("Puntaje: ");
      tft.setCursor(200, 300);
      tft.print(puntaje, 10);

      tft.setCursor(120, 330);
      tft.print("Puntaje max: ");
      tft.setCursor(200, 330);
      tft.print(highScore, 10);
      tft.drawBitmap(120, 200, naveSprite, 16, 16, TFT_WHITE);
      while(digitalRead(buttonPin)){
        tft.drawBitmap(120, 200, naveSprite, 16, 16, TFT_BLACK);
        tft.drawBitmap(120, 200, ExplosionGfx, 16, 16, TFT_WHITE);
        delay(1000);
        tft.drawBitmap(120, 200, ExplosionGfx, 16, 16, TFT_BLACK);
        tft.drawBitmap(120, 200, naveSprite, 16, 16, TFT_WHITE);
        delay(1000);
        if(llamoMenu){
          return;
        }
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
      tft.setCursor(160, 100);
      tft.print(nivel, 10);
    }
};

Juego juego;
unsigned long tiempoT = millis();

void spaceInvaders (){
  EEPROM.get(300, highScore);
  do{
    tft.fillScreen(BLACK); //pone pantalla en negro
    juego.reiniciar();
    while(!juego.get_gameOver() && !llamoMenu){
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
    if(!llamoMenu){
      juego.gameOver();
    }
  }while(!llamoMenu);
  llamoMenu = 0;
  tft.fillScreen(TFT_BLACK);
  printTitulo();
  drawMenu();
}
/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    FIN SPACE INVADERS
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    INICIO MENU
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

void miniTetris(){
  tft.fillRect(120, 300, 100, 100, TFT_GREEN);
  int pieza[4][4] = {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}};
  int pieza2[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}};
  int pieza3[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 0, 0}, {1, 1, 0, 0}};
  draw(pieza, 170, 320, TFT_RED);
  draw(pieza2, 130, 340, TFT_MAGENTA);
  draw(pieza3, 180, 340, TFT_MAGENTA);
  tft.fillRect(130, 380, 80, 10, TFT_MAGENTA);
}

void miniSnake(){
  tft.fillRect(120, 300, 100, 100, TFT_DARKGREEN);
  drawApple(140, 320);
  drawAppleSpecial(180, 340);
  tft.fillRect(130, 360, 60, 4, TFT_WHITE);
}

void miniBreakout(){
  tft.fillRect(120, 300, 100, 100, TFT_BLACK);
  tft.fillRect(130, 320, 20, 8, TFT_RED);
  tft.fillRect(150, 320, 20, 8, TFT_YELLOW);
  tft.fillRect(170, 320, 20, 8, TFT_MAGENTA);
  tft.fillRect(140, 328, 20, 8, TFT_BLUE);
  tft.fillRect(160, 328, 20, 8, TFT_GREEN);
  tft.fillRect(170, 360, 4, 4, TFT_YELLOW);
  tft.fillRect(125, 380, 30, 8, TFT_YELLOW);
}

void miniSpaceInvaders(){
  tft.fillRect(120, 300, 100, 100, TFT_BLACK);
  tft.drawBitmap(150, 300, alienFilaUno, 16, 16, coloresAliens[0]);
  tft.fillRect(158, 340, 2, 4, TFT_WHITE);
  tft.drawBitmap(150, 360, naveSprite, 16, 16, TFT_WHITE);
}

void (*animaciones[])() = {miniTetris, miniSnake, miniBreakout, miniSpaceInvaders};

int menuOption = 0;
// Función para dibujar el menú en la pantalla

void printTitulo(){
  tft.drawBitmap(0, 10, letraA, 32, 32, TFT_PINK);
  tft.drawBitmap(35, 10, letraR, 32, 32, TFT_CYAN);
  tft.drawBitmap(70, 10, letraC, 32, 32, TFT_ORANGE);
  tft.drawBitmap(105, 10, letraA, 32, 32, TFT_GREEN);
  tft.drawBitmap(140, 10, letraD, 32, 32, TFT_BLUE);
  tft.drawBitmap(175, 10, letraU, 32, 32, TFT_RED);
  tft.drawBitmap(210, 10, letraI, 32, 32, TFT_PURPLE);
  tft.drawBitmap(245, 10, letraN, 32, 32, TFT_NAVY);
  tft.drawBitmap(280, 10, letraO, 32, 32, TFT_OLIVE);

  tft.drawBitmap(1, 11, letraA, 32, 32, TFT_PINK);
  tft.drawBitmap(36, 11, letraR, 32, 32, TFT_CYAN);
  tft.drawBitmap(71, 11, letraC, 32, 32, TFT_ORANGE);
  tft.drawBitmap(106, 11, letraA, 32, 32, TFT_GREEN);
  tft.drawBitmap(141, 11, letraD, 32, 32, TFT_BLUE);
  tft.drawBitmap(176, 11, letraU, 32, 32, TFT_RED);
  tft.drawBitmap(211, 11, letraI, 32, 32, TFT_PURPLE);
  tft.drawBitmap(246, 11, letraN, 32, 32, TFT_NAVY);
  tft.drawBitmap(281, 11, letraO, 32, 32, TFT_OLIVE);

  tft.drawBitmap(2, 12, letraA, 32, 32, TFT_PINK);
  tft.drawBitmap(37, 12, letraR, 32, 32, TFT_CYAN);
  tft.drawBitmap(72, 12, letraC, 32, 32, TFT_ORANGE);
  tft.drawBitmap(107, 12, letraA, 32, 32, TFT_GREEN);
  tft.drawBitmap(142, 12, letraD, 32, 32, TFT_BLUE);
  tft.drawBitmap(177, 12, letraU, 32, 32, TFT_RED);
  tft.drawBitmap(212, 12, letraI, 32, 32, TFT_PURPLE);
  tft.drawBitmap(247, 12, letraN, 32, 32, TFT_NAVY);
  tft.drawBitmap(282, 12, letraO, 32, 32, TFT_OLIVE);

  tft.drawBitmap(3, 13, letraA, 32, 32, TFT_PINK);
  tft.drawBitmap(38, 13, letraR, 32, 32, TFT_CYAN);
  tft.drawBitmap(73, 13, letraC, 32, 32, TFT_ORANGE);
  tft.drawBitmap(108, 13, letraA, 32, 32, TFT_GREEN);
  tft.drawBitmap(143, 13, letraD, 32, 32, TFT_BLUE);
  tft.drawBitmap(178, 13, letraU, 32, 32, TFT_RED);
  tft.drawBitmap(213, 13, letraI, 32, 32, TFT_PURPLE);
  tft.drawBitmap(248, 13, letraN, 32, 32, TFT_NAVY);
  tft.drawBitmap(283, 13, letraO, 32, 32, TFT_OLIVE);
}

void drawMenu() {
  //tft.fillScreen(TFT_BLACK); // Limpiar la pantalla

  // Dibujar el título del menú
  //tft.print("Arcaduino");
  
  // Dibujar las opciones del menú
  tft.setTextSize(2);
  for (int i = 0; i < cantidadJuegos; i++) { // Cambia el valor 3 al número de opciones
    if (i == menuOption) {
      tft.setTextColor(TFT_GREEN); // Cambia el color de la opción seleccionada
      animaciones[i]();
    } else {
      tft.setTextColor(TFT_WHITE);
    }
    tft.setCursor(20, 70 + i * 40); // Ajusta las coordenadas de las opciones
    tft.print(pointer[i]); // Cambia el texto de las opciones según tu necesidad
  }
}

void drawIngresoUsuario(){
  bool noIngresoUsuario = false;
  short letraSeleccionada = 0;
  int j = 0;
  int k;
  int l = 0; 
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  for(int m = 0; m < sizeof(nick) / sizeof(nick[0]); m++){
    tft.setCursor(130 + m * 20, 300); // Ajusta las coordenadas de las opciones
    tft.print(nick[m]); // Cambia el texto de las opciones según tu necesidad
  }
  while(!noIngresoUsuario){
    if(analogRead(xPin) >= 600){
      letraSeleccionada++;
    }
    if(analogRead(xPin) <= 400){
      letraSeleccionada--;
    }
    if(analogRead(yPin) >= 600){
      letraSeleccionada -=7;
    }
    if(analogRead(yPin) <= 400){
      letraSeleccionada +=7;
    }
    if(letraSeleccionada < 0){
      letraSeleccionada = 27;
    }
    if(letraSeleccionada > 27){
      letraSeleccionada = 0;
    }
    delay(150);
    k = 0;
    for (int i = 0; i < sizeof(letras) / sizeof(letras[0]); i++) { // Cambia el valor 3 al número de opciones
      if (i == letraSeleccionada) {
        tft.setTextColor(TFT_GREEN); // Cambia el color de la opción seleccionada
      } else {
        tft.setTextColor(TFT_WHITE);
      }
      if(!digitalRead(buttonPin)){
        if(letras[letraSeleccionada] == ">"){
          noIngresoUsuario = true;
          break;
        }
        if(letras[letraSeleccionada] == "<"){
          nick[l] = "_";
          l--;
          if( l < 0 ){
            l = 0;
          }
        }
        if(letras[letraSeleccionada] != ">" && letras[letraSeleccionada] != "<"){
          nick[l] = letras[letraSeleccionada];
          l++;
          if(l > 2){
            l = 2;
          }
        }
        tft.fillRect(130,300, 60 , 20,TFT_BLACK);
        for(int m = 0; m < sizeof(nick) / sizeof(nick[0]); m++){
          tft.setCursor(130 + m * 20, 300); // Ajusta las coordenadas de las opciones
          tft.print(nick[m]); // Cambia el texto de las opciones según tu necesidad
        }
        delay(150);
      }
      tft.setCursor(90 + j * 20, 70 + k * 40); // Ajusta las coordenadas de las opciones
      tft.print(letras[i]); // Cambia el texto de las opciones según tu necesidad
      j++;
      if(j == 7){
        k++;
        j = 0;
      }
    }
  }
  tft.fillScreen(TFT_BLACK);
}

/*
****************************************************************************************************************************************************
****************************************************************************************************************************************************
    FIN MENU
****************************************************************************************************************************************************
****************************************************************************************************************************************************
*/

void alMenu(){
  Serial.println("Entre");
  llamoMenu = 1;
}

void setup() {
  //algunas configuraciones de la pantalla / joystick inicales
  g_identifier = tft.readID();
  Serial.begin(9600);
  //Serial.print("ID = 0x");
  //Serial.println(g_identifier, HEX);
  tft.begin(g_identifier);
  tft.setRotation(0); //pantalla en vertical
  tft.fillScreen(TFT_BLACK);
  attachInterrupt(digitalPinToInterrupt(21), alMenu, RISING);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  //activar resistencia pull-up en el pin pulsador
  pinMode(buttonPin, INPUT_PULLUP);
  juego = Juego();
  printTitulo();
  drawIngresoUsuario();
  printTitulo();
  drawMenu();
}

void loop() { //bucle del juego
    // Verificar la dirección del joystick para cambiar la opción del menú
    if (analogRead(yPin) > 700) {
      menuOption--;
      if (menuOption < 0) {
        menuOption = cantidadJuegos - 1; // Cambia este valor al número de opciones menos uno
      }
      drawMenu();
    } else if (analogRead(yPin) < 400) {
      menuOption++;
      if (menuOption > cantidadJuegos - 1) { // Cambia este valor al número de opciones menos uno
        menuOption = 0;
      }
      drawMenu();
    }

    // Verificar si se presiona el botón del joystick para seleccionar la opción
    if (!digitalRead(buttonPin)) {
      // Realizar acciones basadas en la opción seleccionada
      switch (menuOption) {
        case 0:
          tetris();
          break;
        case 1:
          tft.setRotation(1);
          snake();// Acción para la opción 1
          break;
        case 2:
          // Acción para la opción 2
          breakout();
          break;
        case 3:
          // Acción para la opción 3
          spaceInvaders();
          break;
        // Agrega más casos según las opciones de tu menú
      }
    }
    delay(100);
}
