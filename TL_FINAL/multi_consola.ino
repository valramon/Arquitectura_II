#include <Arduino.h>
#include <MCUFRIEND_kbv.h> //libreria de la pantalla
#include <Adafruit_GFX.h>
#include "protothreads.h"
//#include <SPI.h>
//pin definiciones
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// change this to whichever pin you want to use
int buzzer = 33;
MCUFRIEND_kbv tft;// = MCUFRIEND_kbv(LCD_CS, LCD_CD, LCD_WR, LCD_CS, LCD_RESET); //la pantallita sera tft
int16_t g_identifier;


//variables para el joystick
int xPin = A8; //EJE x
int yPin = A9; //EJE Y
int buttonPin = 39; //Apretar botón
int llamoMenu = 0;
char *pointer[] = { "Tetris", "Snake",  "Pr \u00F3 ximamente"};

/*
  *************
  *************
    INICIO MENU
  *************
  *************
*/
int menuOption = 0;
// Función para dibujar el menú en la pantalla
void drawMenu() {
  //tft.fillScreen(TFT_BLACK); // Limpiar la pantalla

  // Dibujar el título del menú
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Menu de Seleccion");

  // Dibujar las opciones del menú
  tft.setTextSize(2);
  for (int i = 0; i < 3; i++) { // Cambia el valor 3 al número de opciones
    if (i == menuOption) {
      tft.setTextColor(TFT_GREEN); // Cambia el color de la opción seleccionada
    } else {
      tft.setTextColor(TFT_WHITE);
    }
    tft.setCursor(20, 70 + i * 40); // Ajusta las coordenadas de las opciones
    tft.print(pointer[i]); // Cambia el texto de las opciones según tu necesidad
  }
  tft.setTextColor(TFT_WHITE);
}

/*
  *************
  *************
    FIN MENU
  *************
  *************
*/


/*
  *************
  *************
    INICIO TETRIS
  *************
  *************
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
int tempo=144; 



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
  tft.setCursor(50, 150); //nos ponemos en el pixel 50x150
  tft.setTextSize(4); //el texto sera de tamaño 4
  tft.setTextColor(GREEN);
  tft.println("Tetris!");
  delay(2000);
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
  noTone(buzzer);
  tft.fillRect(70, 0, 160, 5, TFT_CYAN);

  // Dibuja un rectángulo de color BLACK en el resto de la pantalla
  tft.fillRect(70, 6, 160, 305, TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 350);
  tft.setTextSize(3);
  tft.print("Puntaje:");
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(150,350),
  tft.print(puntaje,10);
  tone(buzzer, 700, 200);
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
  delay(1000);
  noTone(buzzer);
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
  delay(5000);
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
  unsigned int tiempoActual = millis();
  int randomPiezaDespues = random()%7;
  int randomPiezaAct;
  int thisNote = 0;
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
    //bucle que hace que piezas vayan bajando por la pantalla
    for (int i = 5; (i<tft.height()-24) && (!toca); i+=10) {
      posY = i;
      Serial.println(posY);
      Serial.println(posYBorrar);
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

      // we only play the note for 90% of the duration, leaving 10% as a pause
      tone(buzzer, melody[thisNote], noteDuration*0.9);
      delay(100);
      // Wait for the specief duration before playing the next note.
      if(millis() > tiempoActual + noteDuration * 1.5){
        tiempoActual = millis() - 150;
        noTone(buzzer);
        thisNote = thisNote + 2;
        if (thisNote >= notes){
          thisNote = 0;
        }
      }
      // stop the waveform generation before the next note.
      
      
      //volvemos a mirar la rotacion
      cont++; //actualizar contador de filas
      
      eliminarFilas(); //por si hubiera una fila completa --> eliminarla
      //por si se hubiera acabado el juego
      if (finPartida()) {
        mostrarMensajeFin();
        tetris();
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
    pantallaInicio();
    //dibujarPantalla();
    inicicializarMatrizGame();
    dibujoInicial();
    reiniciarPantalla();
    inicioJuego();
  }while(!llamoMenu);
  llamoMenu = 0;
  tft.fillScreen(TFT_BLACK);
  drawMenu();
}

/*
  *************
  *************
    FIN TETRIS
  *************
  *************
*/

/*
  *************
  *************
    INICIO SNAKE
  *************
  *************
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
  drawMenu();
}

/*
  *************
  *************
    FIN SNAKE
  *************
  *************
*/

void alMenu(){
  llamoMenu = 1;
}

void setup() {
  //algunas configuraciones de la pantalla / joystick inicales
  g_identifier = tft.readID();
  //Serial.begin(9600);
  //Serial.print("ID = 0x");
  //Serial.println(g_identifier, HEX);
  tft.begin(g_identifier);
  tft.setRotation(0); //pantalla en vertical
  tft.fillScreen(TFT_BLACK);
  attachInterrupt(2, alMenu, FALLING);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  //activar resistencia pull-up en el pin pulsador
  pinMode(buttonPin, INPUT_PULLUP);
  drawMenu();
}

void loop() { //bucle del juego
    // Verificar la dirección del joystick para cambiar la opción del menú
    if (analogRead(yPin) > 700) {
      menuOption--;
      if (menuOption < 0) {
        menuOption = 2; // Cambia este valor al número de opciones menos uno
      }
      drawMenu();
    } else if (analogRead(yPin) < 400) {
      menuOption++;
      if (menuOption > 2) { // Cambia este valor al número de opciones menos uno
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
          break;
        // Agrega más casos según las opciones de tu menú
      }
    }
    delay(200);
}
