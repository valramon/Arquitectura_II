#include <Adafruit_GFX.h>     // Biblioteca para gráficos
#include <Arduino.h>
#include <MCUFRIEND_kbv.h> //libreria de la pantalla

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Crear una instancia de la pantalla TFT
MCUFRIEND_kbv tft;
int16_t g_identifier;

//variables para el joystick
int xPin = A8; //EJE x
int yPin = A9; //EJE Y
int buttonPin = 53; //Apretar botón

int xPosition = 0;
int yPosition = 0;
int buttonState = 1;

int menuOption = 0; // Opción del menú seleccionada

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
  // Configurar las opciones del menú
  drawMenu();
}

void loop() {
  // Verificar la dirección del joystick para cambiar la opción del menú
  if (analogRead(xPin) < 500) {
    menuOption--;
    if (menuOption < 0) {
      menuOption = 2; // Cambia este valor al número de opciones menos uno
    }
    drawMenu();
  } else if (analogRead(xPin) > 600) {
    menuOption++;
    if (menuOption > 2) { // Cambia este valor al número de opciones menos uno
      menuOption = 0;
    }
    drawMenu();
  }

  // Verificar si se presiona el botón del joystick para seleccionar la opción
  if (digitalRead(buttonPin)) {
    // Realizar acciones basadas en la opción seleccionada
    switch (menuOption) {
      case 0:
        // Acción para la opción 0
        break;
      case 1:
        // Acción para la opción 1
        break;
      case 2:
        // Acción para la opción 2
        break;
      // Agrega más casos según las opciones de tu menú
    }
  }
  delay(200);
}

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
    tft.print("Opcion " + String(i)); // Cambia el texto de las opciones según tu necesidad
  }
  tft.setTextColor(TFT_WHITE);
}