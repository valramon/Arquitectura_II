
/*===========================================================================*/

#include "sapi.h"        // <= Inclusion de la Biblioteca sAPI

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
     // ---------- CONFIGURACIONES ------------------------------

    boardConfig(); // Inicializar y configurar la plataforma
    uartConfig( UART_USB, 115200 ); // Inicializar periferico UART_USB

    // Inicializar el sensor ultrasonico HC-SR04
    ultrasonicSensorConfig( ULTRASONIC_SENSOR_0, ULTRASONIC_SENSOR_ENABLE );
    gpioConfig(GPIO1 , GPIO_OUTPUT) ; // Se c o n f i g u r a e l GPIO1 como s a l i d a
    gpioConfig(GPIO3 , GPIO_OUTPUT) ; // Se c o n f i g u r a e l GPIO3 como s a l i d a
    gpioConfig(GPIO4 , GPIO_OUTPUT) ; // Se c o n f i g u r a e l GPIO4 como s a l i d a
    gpioConfig(GPIO6 , GPIO_OUTPUT) ; // Se c o n f i g u r a e l GPIO6 como s a l i d a
    delay(100); // Retardo inicial de 100 ms

    // Variables enteras de 32 bits para guardar la distancia en pulgadas y centimetros
    uint32_t distanceInCms;

   // ---------- REPETIR POR SIEMPRE --------------------------
	while(TRUE)
	{
		// Obtenemos la distancia actual medida por el sensor en centimetros
		distanceInCms = ultrasonicSensorGetDistance(ULTRASONIC_SENSOR_0, CM);

        	gpioWrite(GPIO4 , true) ;
       		gpioWrite(GPIO6 , false) ;
		// Condicion de sentido de giro de la rueda
        	if (distanceInCms > 15) {
            		// Sigo el sentido de la otra rueda
			printf( "Lejos - Distance: %d cm \".\r\n", distanceInCms );
            		gpioWrite(GPIO1 , true) ;
            		gpioWrite(GPIO3 , false) ;
        	}
        	else{
            		// Giro en sentido contrario de la otra rueda
			printf( "Muy cerca - Distance: %d cm \".\r\n", distanceInCms );
            		gpioWrite(GPIO1 , false) ;
            		gpioWrite(GPIO3 , true) ;
        	}
	}
   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamente sobre un microcontrolador y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}
