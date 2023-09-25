/* Copyright 2017, Eric Pernia.
 * Copyright 2017, Pablo Gomez.
 * Copyright 2017, Eric Pernia.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
 /*
 * Date: 2017-04-14 
 */

/*==================[inlcusiones]============================================*/

#include "sapi.h" // Biblioteca sAPI
#include "stdlib.h"

/*=================[definicion de valores de botones]========================*/
const char valorBotonT_FIL2 = '1';
const char valorBotonT_FIL3 = '2';
const char valorBotonT_FIL0 = '3';
const char valorBotonT_COL1 = '4';

/*==================[definiciones y macros]==================================*/

#define ANODO_COMUN 1
#define CHARS 26
#define SERVO_N   SERVO0
#define TCOL_1 1

/*==================[definiciones de datos internos]=========================*/

static uint8_t k=0;
static delay_t refrescarDisplay;
static delay_t cambiarDigito;
bool_t asd;

uint8_t charsVec[CHARS] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','A','C','E','H','J','L','P','U'};

/*
     D1   a   f  D2  D3   b
     12  11  10   9   8   7
      |   |   |   |   |   |
   +--+---+---+---+---+---+--+
   |    _     _     _     _  |
   |  /__/  /__/  /__/  /__/ |
   | /_ /. /_ /. /_ /. /_ /. |
   |                         |
   +--+---+---+---+---+---+--+
      |   |   |   |   |   |
      1   2   3   4   5   6
      e   d   h   c   g  D4
*/

// Configuracion de pines de segmentos del display
uint8_t display7SegmentosPines[8] = {
   GPIO4, // a
   GPIO5, // b
   GPIO6, // c
   GPIO2, // d
   GPIO1, // e
   GPIO3, // f
   GPIO7, // g
   GPIO8  // h (dp) 
};

// Configuracion de pines de digitos del display
uint8_t display7SegmentosPinesDigitos[4] = {
   LCD1, // D1
   LCD2, // D2
   LCD3, // D3
   LCD4  // D4
};

uint8_t display7SegmentosSalidas[CHARS] = {
   0b00111111, // 0
   0b00000110, // 1
   0b01011011, // 2
   0b01001111, // 3
   0b01100110, // 4
   0b01101101, // 5
   0b01111101, // 6
   0b00000111, // 7
   0b01111111, // 8
   0b01101111, // 9

   0b01011111, // a
   0b01111100, // b
   0b01011000, // c
   0b01011110, // d
   0b01111011, // e
   0b01110001, // f

   0b01110111, // A
   0b00111001, // C
   0b01111001, // E
   0b01110110, // H
   0b00011110, // J
   0b00111000, // L
   0b01110011, // P
   0b00111110, // U

   0b10000000, // .
   0b00000000  // blank
};

/*==================[definiciones de funciones internas]=====================*/

void display7SegmentosEscribir( uint8_t symbolIndex ){

   uint8_t i = 0;

   for(i=0;i<=7;i++)
   #ifdef ANODO_COMUN
      gpioWrite( display7SegmentosPines[i], !(display7SegmentosSalidas[symbolIndex] & (1<<i)) );
   #elif defined(CATODO_COMUN)
      gpioWrite( display7SegmentosPines[i], (display7SegmentosSalidas[symbolIndex] & (1<<i)) );
   #endif   
}

uint8_t charToIndex( uint8_t charIndex ){

   uint8_t output = CHARS - 1;
   uint8_t i;
   
   for(i=0;i<CHARS;i++) {
      if (charIndex == charsVec[i])
         output = i;
   }
   return output; 
}

void display7SegmentosEscribirDigito( uint8_t digitIndex ){

   uint8_t i = 0;

   for(i=0;i<=3;i++)
   #ifdef ANODO_COMUN
      gpioWrite( display7SegmentosPinesDigitos[i], OFF );
      gpioWrite( display7SegmentosPinesDigitos[digitIndex], ON);    
   #elif defined(CATODO_COMUN)
      gpioWrite( display7SegmentosPinesDigitos[i], ON );
      gpioWrite( display7SegmentosPinesDigitos[digitIndex], OFF);
   #endif   
}

char* itoaDisplay(int value, char* result, int base) {

    result[3] = (value % 10) + '0';
    result[2] = ((value % 100) / 10) + '0';
    result[1] = ((value % 1000) / 100) + '0';
    result[0] = ((value % 10000) / 1000) + '0';
    
    return result;
}

void display7SegmentosConfigurarPines( void ){
   uint8_t i = 0;
   
   delayConfig( &refrescarDisplay, 3 );
   delayConfig( &cambiarDigito, 500 );
   
   for(i=0;i<=7;i++)
       gpioConfig( display7SegmentosPines[i], GPIO_OUTPUT );
       
   for(i=0;i<=3;i++)
       gpioConfig( display7SegmentosPinesDigitos[i], GPIO_OUTPUT );
}

void display7SegmentosEscribir4Digitos( uint8_t* digit ){

   if( delayRead(&refrescarDisplay) ){

      display7SegmentosEscribirDigito(k);
      
      display7SegmentosEscribir( charToIndex(digit[k]) );
      k++;
      if(k>4)
         k=0;
   }
}

/*================== Funciones del reinicio del sistema embebido ==================*/

void reiniciarDisplay(uint8_t* v){
    for(int i =0; i<4; i++){
        v[i]='0';
    }
}

void prenderLedsIntentos(){
    gpioWrite(T_COL2, 0);
    gpioWrite(T_COL0, 0);
    gpioWrite( LED1, 1 );
    gpioWrite( LED2, 1 );
    gpioWrite( LED3, 1 );
    gpioWrite( LEDB, 1 );
    gpioWrite( LEDR, 1 );
}

void reiniciarServo(){
    servoWrite( SERVO_N, 0 );
}

void reiniciarSistema( uint8_t* v, int* cantidadIntentos){
    reiniciarDisplay(v);
    reiniciarLeds();
    reiniciarServo();
    *cantidadIntentos = 4;
}

/*================== Funciones de control del sistema embebido ==================*/

void intentosTerminados( uint8_t* v, int* cantidadIntentos){
    while(gpioRead(CAN_TD) == 0){
        display7SegmentosEscribir4Digitos(v);
    }
    reiniciarSistema(v,cantidadIntentos);
}

void descontandoIntentos(int cantidadIntentos,  uint8_t* v ){
    switch(cantidadIntentos){
        case 3:
            gpioWrite(LED3, 0);
            break;
        case 2:
            gpioWrite(LED2, 0);
            break;
        case 1:
            gpioWrite(LED1, 0);
            break;
        case 0:
            gpioWrite(T_COL0, 1);
            gpioWrite(LEDR, 0);
            gpioWrite(LEDB, 0);
            break;
        default:
            break;
    }
}

void desbloqueoCerradura(int* aciertos, int* cantidadIntentos){
    servoWrite( SERVO_N, 90 );
    gpioWrite(T_COL2, 1);
    aciertos = 0;
    cantidadIntentos = 0;
}

void falloIntento(uint8_t* v, int* cantidadIntentos){
    cantidadIntentos--;
    descontandoIntentos(cantidadIntentos, v);
    reiniciarDisplay(v);
}

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){

    // ----------------------- CONFIGURACIONES --------------------------- //
    // Inicializar y configurar la plataforma //
    boardConfig();   
    display7SegmentosConfigurarPines(); // Configuracion de pines para el display 7 segmentos
    gpioConfig(T_FIL2 , GPIO_INPUT) ; // Se c o n f i g u r a  e l T_FIL2 como e n t r a d a
    gpioConfig(T_FIL3, GPIO_INPUT) ; // Se c o n f i g u r a  e l T_FIL3 como e n t r a d a
    gpioConfig(T_FIL0 , GPIO_INPUT) ; // Se c o n f i g u r a  e l T_FIL0 como e n t r a d a
    gpioConfig(T_COL1, GPIO_INPUT) ; // Se c o n f i g u r a  e l T_COL1 como e n t r a d a
    gpioConfig(T_COL0, GPIO_OUTPUT) ; // Se c o n f i g u r a  e l T_COL0 como s a l i d a
    gpioConfig(T_COL2, GPIO_OUTPUT) ; // Se c o n f i g u r a  e l T_COL2 como s a l i d a
    gpioConfig(CAN_TD, GPIO_INPUT); // Se c o n f i g u r a  e l CAN_TD como e n t r a d a
    uint8_t v[4] = {'0','0','0','0'}; // Nuestro display 7 segmentos
    uint8_t combinacionCorrecta[4] = {'3', '3', '3', '3'}; // contrasenia a decifrar
    int cantidadIntentos = 4; // cantidad de intentos posibles
    int aciertos = 0; // cantidad de aciertos ingresados
    bool_t valor = 0;

    uint8_t servoAngle = 0; // 0 a 180 grados
    int posicionDisplay = 0;
    // Configurar Servo
    valor = servoConfig( 0, SERVO_ENABLE );

    valor = servoConfig( SERVO_N, SERVO_ENABLE_OUTPUT );

    // Usar Servo
    valor = servoWrite( SERVO_N, servoAngle );
    servoAngle = servoRead( SERVO_N );

    gpioWrite( LED1, 1 );
    gpioWrite( LED2, 1 );
    gpioWrite( LED3, 1 );
    gpioWrite( LEDB, 1 );
    gpioWrite( LEDR, 1 );
   
   // ---------- REPETIR POR SIEMPRE -------------------------- //
   // ----------------- Logica -----------------
   while( TRUE ){
        display7SegmentosEscribir4Digitos(v);
        if(gpioRead(T_FIL2) == 1){
            v[posicionDisplay] = valorBotonT_FIL2;
            posicionDisplay++;
            delay(500);
        }
        if(gpioRead(T_FIL3) == 1){
            v[posicionDisplay] = valorBotonT_FIL3;
            posicionDisplay++;
            delay(500);
        }
        if(gpioRead(T_FIL0) == 1){
            v[posicionDisplay] = valorBotonT_FIL0;
            posicionDisplay++;
            delay(500);
        }
        if(gpioRead(T_COL1) == 1){
            v[posicionDisplay] = valorBotonT_COL1;
            posicionDisplay++;
            delay(500);
        }
        if(posicionDisplay == 4){
            for(int i = 0; i < posicionDisplay; i++){
                if(v[i] != combinacionCorrecta[i]){
                    falloIntento(v,&cantidadIntentos);
                    break;
                }
                else {aciertos++;}
            }
            if(aciertos == 4){
                desbloqueoCerradura(&aciertos,&cantidadIntentos);
            }
            posicionDisplay = 0;
        }
        if(cantidadIntentos == 0){
            intentosTerminados(v, &cantidadIntentos);
        }
   } 
   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta 
   // directamenteno sobre un microcontroladore y no es llamado/ por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}
/*==================[fin del archivo]========================================*/
