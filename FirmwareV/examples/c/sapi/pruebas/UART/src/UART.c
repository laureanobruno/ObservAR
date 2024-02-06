#include "sapi.h" // <= sAPI header
#define SERVO_N SERVO0

void main(void)
{

    valor = servoConfig(0, SERVO_ENABLE);

    valor = servoConfig(SERVO_N, SERVO_ENABLE_OUTPUT);

    gpioWrite(LEDB, 1);

    gpioWrite(LEDB, 1);

    boardConfig(); // Initialize board

    uartConfig(UART_USB, 115200); // Initialize UART_USB at 115200 baud rate
    uartConfig(UART_232, 9600);   // Initialize UART_232 (which we'll use for communication) at 9600 baud rate

    int servoAngle = 0; // Initialize the variable to store the servo angle

    // Usar Servo
    // valor = servoWrite(SERVO_N, servoAngle);

    int i = 0;

    while (1)
    {
        char receivedByte;
        char[2] recievedString;

        if (uartReadByte(UART_232, &receivedByte))
        { // Read data from UART_232
            if (receivedByte == '\n')
            {
                servoAngle = atoi(recievedString);
                servoWrite(SERVO_N, servoAngle);
                i = 0;
            }
            else
            {
                recievedString[i] = receivedByte;
                uartWriteByte(UART_USB, receivedByte); // Write the received byte to UART_USB for debugging
            }
            i++;
        }
    }
}
