/*============================================================================
 * Autor:
 * Licencia:
 * Fecha:
 *===========================================================================*/

// Inlcusiones

// #include "app.h"         // <= Su propia cabecera
// #include "sapi.h"        // <= Biblioteca sAPI
//
//// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
// int main( void )
//{
//    // ---------- CONFIGURACIONES ------------------------------
//
//    // Inicializar y configurar la plataforma
//    boardConfig();
//
//    // Crear varias variables del tipo booleano
//    bool_t buttonValue = OFF;
//    bool_t ledValue    = OFF;
//    // Crear variable del tipo tick_t para contar tiempo
//    tick_t timeCount   = 0;
//
//    // ---------- REPETIR POR SIEMPRE --------------------------
//    while( TRUE ) {
//
//       /* Retardo bloqueante durante 100ms */
//
//       delay( 100 );
//
//       /* Si pasaron 10 segundos comienza a funcionar el programa que copia las
//          acciones en BOTON al LED. Mientras espera titila el LED.  */
//
//       timeCount++;
//
//       if( timeCount == 100 ){ // 100ms * 100 = 10s
//
//          while( TRUE ) {
//
//             /* Si se presiona CIAA_BOARD_BUTTON, enciende el CIAA_BOARD_LED */
//
//             // Leer pin conectado al boton.
//             buttonValue = gpioRead( CIAA_BOARD_BUTTON );
//             // Invertir el valor leido, pues lee un 0 (OFF) con boton
//             // presionado y 1 (ON) al liberarla.
//             buttonValue = !buttonValue;
//             // Escribir el valor leido en el LED correspondiente.
//             gpioWrite( CIAA_BOARD_LED, buttonValue );
//
//             /* Enviar a la salida estandar (UART_DEBUG) el estado del LED */
//
//             // Leer el estado del pin conectado al led
//             ledValue = gpioRead( CIAA_BOARD_LED );
//             // Chequear si el valor leido es encedido
//             if( ledValue == ON ) {
//                // Si esta encendido mostrar por UART_USB "LED encendido."
//                printf( "LED encendido.\r\n" );
//             } else {
//                // Si esta apagado mostrar por UART_USB "LED apagado."
//                printf( "LED apagado.\r\n" );
//             }
//             delay( 250 );
//
//          }
//       } else {
//          // Intercambiar el valor de CIAA_BOARD_LED
//          gpioToggle(CIAA_BOARD_LED);
//       }
//    }
//
//    // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
//    // directamenteno sobre un microcontroladore y no es llamado por ningun
//    // Sistema Operativo, como en el caso de un programa para PC.
//    return 0;
// }

// #include "sapi.h"  // <= sAPI header

// void main(void) {
//     boardConfig();  // Initialize board

//     uartConfig(UART_USB, 9600);  // Initialize UART_USB at 115200 baud rate
//     uartConfig(UART_232, 115200);    // Initialize UART_232 (which we'll use for communication) at 9600 baud rate

//     while(1) {
//         char receivedByte;
//         if(uartReadByte(UART_232, &receivedByte))
//         {  // Read data from UART_232
//             uartWriteByte(UART_USB, receivedByte);  // Write the received byte to UART_USB for debugging
//         }
//     }
// }

#include "sapi.h" // <= sAPI header
#define SERVO_N   SERVO0

int atoi(char *str)
{
    int res = 0; // Initialize result

    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // return result.
    return res;
}

void main(void)
{
	boardConfig();

    bool_t valor = 0;

    uint8_t servoAngle = 0; // 0 a 180 grados

    gpioToggle(LEDB);
    delay( 500 );

    valor = servoConfig( 0, SERVO_ENABLE );

    valor = servoConfig( SERVO_N, SERVO_ENABLE_OUTPUT );

    gpioToggle(LED1);
    delay( 500 );

    //uartConfig(UART_USB, 9600); // Initialize UART_USB at 115200 baud rate
    //uartConfig(UART_232, 115200);   // Initialize UART_232 (which we'll use for communication) at 9600 baud rate


    gpioToggle(LED2);
    delay( 500 );

    //int servoAngle = 0; // 0 a 180 grados
    // Usar Servo
    // valor = servoWrite(SERVO_N, servoAngle);

    int i = 0;

    while (1)
    {
        char receivedByte;
        // string where we'll store the characters received
        char recievedString[2] = {0, 0};

        if (uartReadByte(UART_232, &receivedByte))
        { // Read data from UART_232
            if (receivedByte == '\n')
            {
                servoAngle = atoi(recievedString);
                //servoWrite(SERVO_N, servoAngle);
                i = 0;
            }
            else
            {
                recievedString[i] = receivedByte;
            }
            //uartWriteByte(UART_USB, receivedByte); // Write the received byte to UART_USB for debugging
            uartWriteByte(UART_USB, 'A');
            i++;
        }
    }
}
