#include <EEPROM.h>
#include <Adafruit_GFX.h>     // Biblioteca para gráficos
#include <Arduino.h>
#include <MCUFRIEND_kbv.h> //libreria de la pantalla

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin


#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5


// Crear una instancia de la pantalla TFT
MCUFRIEND_kbv tft;
int16_t g_identifier;

//variables para el joystick
int xPin = A8; //EJE x
int yPin = A9; //EJE Y
int buttonPin = 39; //Apretar botón

int xPosition = 0;
int yPosition = 0;
int buttonState = 1;

int menuOption = 0; // Opción del menú seleccionada

//int button=6;
uint16_t pixelColor;
uint8_t buttonPressed, currentDirection;
  byte newPos[2];
  byte oldPos[2];
int ancho = 479;
int altura = 319;
boolean collision, appleOn, displayEnd, specialOn, allowSpecial, showTitle = true;
int head, tiempo, snakeSize, puntaje, pixelLocationXAxis, pixelLocationYAxis, m[300], n[300],
    appleX, appleY, yMultiplier, selection = 100, difficulty, specialX, specialY, specialTime;


void setup() {
  //algunas configuraciones de la pantalla / joystick inicales
  g_identifier = tft.readID();
  //Serial.print("ID = 0x");
  //Serial.println(g_identifier, HEX);
  tft.begin(g_identifier);
  tft.setRotation(1); // Ajustar la orientación de la pantalla si es necesario
  tft.fillScreen(TFT_BLACK); // Llenar la pantalla de negro al inicio
  tft.setTextColor(TFT_WHITE); // Establecer el color del texto
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  //activar resistencia pull-up en el pin pulsador
  pinMode(buttonPin, INPUT_PULLUP);

}

void loop() {
    vibora();
    while(1){
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
            vibora();
      }
            
      }
      delay(difficulty);
      
    }
    

}

uint8_t readButton(void) {
  if (analogRead(yPin)<400) return BUTTON_DOWN;
  if (analogRead(xPin)>700) return BUTTON_RIGHT;
  if (!digitalRead(buttonPin)) return BUTTON_SELECT;
  if (analogRead(yPin)>700) return BUTTON_UP;
  if (analogRead(xPin)<400) return BUTTON_LEFT;
  else return BUTTON_NONE;
}

void showTitlScreen(){
  tft.fillRect(0,0,480,320, TFT_BLUE);
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
    tft.setTextColor((255,0,0));
    //tft.fillRect(25, 75, 50, 18);
    tft.setCursor(60, 80);
    tft.print("Easy");
    tft.setTextColor((0,0,255));
    //tft.fillRect(25, 95, 50, 18);
    tft.setCursor(60, 100);
    tft.print("Medium");
    tft.setTextColor((0,0,255));
    //tft.fillRect(25, 115, 50, 18);
    tft.setCursor(60, 120);
    tft.print("Hard");
    difficulty = 60;
  } else if (selection % 3 == 1){ // normal mode
    tft.setTextColor((0,0,255));
    //tft.fillRect(25, 75, 50, 18);
    tft.setCursor(60, 80);
    tft.print("Easy");
    tft.setTextColor((255,0,0));
    //tft.fillRect(25, 95, 50, 18);
    tft.setCursor(60, 100);
    tft.print("Medium");
    tft.setTextColor((0,0,255));
    //tft.fillRect(25, 115, 50, 18);
    tft.setCursor(60, 120);
    tft.print("Hard");
    difficulty = 40;
  } else { // hard mode
    tft.setTextColor((0,0,255));
    //tft.fillRect(25, 75, 50, 18);
    tft.setCursor(60, 80);
    tft.print("Easy");
    tft.setTextColor((0,0,255));
    //tft.fillRect(25, 95, 50, 18);
    tft.setCursor(60, 100);
    tft.print("Medium");
    tft.setTextColor((255,0,0));
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
void vibora(){
    //tft.InitLCD();
  tft.fillScreen(TFT_BLACK);
  if (showTitle)
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
