#include <Arduino.h>
#include <MCUFRIEND_kbv.h> //libreria de la pantalla
#include <Adafruit_GFX.h>
//#include <SPI.h>
//pin definiciones
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
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

// change this to whichever pin you want to use
int buzzer = 33;

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



MCUFRIEND_kbv tft;// = MCUFRIEND_kbv(LCD_CS, LCD_CD, LCD_WR, LCD_CS, LCD_RESET); //la pantallita sera tft
int16_t g_identifier;

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

//variables para el joystick
int xPin = A8; //EJE x
int yPin = A9; //EJE Y
int buttonPin = 39; //Apretar botón

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
      if (yPosition < 400) {
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
      if (xPosition < 400) {
        moverIzquierda(pieza, cont, columna);
      } else  if (xPosition > 700) {
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
      if(millis() > tiempoActual + noteDuration){
        tiempoActual = millis();
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
        return;
      }
    }
  }
}


void setup() {
  //algunas configuraciones de la pantalla / joystick inicales
  g_identifier = tft.readID();
  //Serial.begin(9600);
  //Serial.print("ID = 0x");
  //Serial.println(g_identifier, HEX);
  tft.begin(g_identifier);
  tft.setRotation(0); //pantalla en vertical
  //attachInterrupt(2, rotaPieza, FALLING);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  //activar resistencia pull-up en el pin pulsador
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() { //bucle del juego
    tft.fillScreen(BLACK); //pone pantalla en negro
    pantallaInicio();
    //dibujarPantalla();
    inicicializarMatrizGame();
    dibujoInicial();
    reiniciarPantalla();
    inicioJuego();
}
