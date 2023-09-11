
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
    pwmConfig( 0, PWM_ENABLE );
    pwmConfig( PWM1, PWM_ENABLE_OUTPUT ); 
    pwmConfig( PWM2, PWM_ENABLE_OUTPUT );
    pwmConfig( PWM3, PWM_ENABLE_OUTPUT );
    pwmConfig( PWM4, PWM_ENABLE_OUTPUT );
    delay(100); // Retardo inicial de 100 ms

    // Variables enteras de 32 bits para guardar la distancia en pulgadas y centimetros
    uint32_t distanceInCms;
    int valorActivoMotor1 = 210;
    int valorBajoMotor = 0;
    int valorActivoMotor2 = 220;

    // ---------- REPETIR POR SIEMPRE --------------------------
	while(TRUE)
	{
		// Obtenemos la distancia actual medida por el sensor en centimetros
		distanceInCms = ultrasonicSensorGetDistance(ULTRASONIC_SENSOR_0, CM);

        	pwmWrite( PWM1,  valorActivoMotor2);
        	pwmWrite( PWM2, valorBajoMotor);
        	if (distanceInCms > 15) {
			// Sigue el sentido de la otra rueda
            		printf( "Lejos - Distance: %d cm \".\r\n", distanceInCms );
            		pwmWrite( PWM3,  valorActivoMotor1);
            		pwmWrite( PWM4, valorBajoMotor);
        	}
        	else{
			// Gira en sentido contrario de la otra rueda
            		printf( "Muy cerca - Distance: %d cm \".\r\n", distanceInCms );
            		pwmWrite( PWM3, valorBajoMotor);
            		pwmWrite( PWM4, valorActivoMotor1);
        	}
	}
   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamente sobre un microcontrolador y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}
