#define SERVO_N SERVO1

#include "sapi.h"
#include "buffer.h"
#include <math.h>
#include <stdlib.h>

#define BTN_START GPIO5
#define BTN_STOP GPIO3
#define BTN_RESET GPIO1

void BTN_config();
void initializeUART();
bool_t initializeServo();

char volatile stringReceived = 0; // Flag de dato recibido por UART

void onRx(void *noUsado)
{
    char c = uartRxRead(UART_232);
    if (c == '\n')
    {
        stringReceived += 1;
    }
    else
    {
        bufferPush(c);
    }
}

void main(void)
{

    boardConfig();
    BTN_config();     // Configuro los botones con pull-up interno
    initializeUART(); // Inicializo UART_USB y UART_232
    bool_t servoConfig = initializeServo();


    bool_t moving = 1; // Flag de movimiento e interrupciones de UART
    servoWrite(SERVO_N, 90);
    uint8_t anguloAct = 90;

    while (1)
    {
        if (!gpioRead(BTN_START))
        {
            moving = 1;
            uartInterrupt(UART_232, true);
            gpioWrite(LED1, 0);
        }
        if (!gpioRead(BTN_STOP))
        {
            moving = 0;
            uartInterrupt(UART_232, false);
            gpioWrite(LED1, 1);
        }
        if (!gpioRead(BTN_RESET))
        {
            servoWrite(SERVO_N, 90);
            anguloAct=90;
            gpioToggle(LED2);
        }

        if (moving)
        {

            if (stringReceived >= 1)
            {

                uint8_t valor = 0;
                uint8_t c = bufferPop();
                while (c != '\r')
                {
                    valor = (valor * 10) + (c - '0');
                    c = bufferPop();
                }

                if (valor == 176) gpioToggle(LEDB);
                else
                {
					// Asumiendo valor entre 4 y 44 Espejado en X
					int8_t alfa = (atan(abs(valor - 24) * 0.017)) * (57.295/2);
					if (valor >= 24) alfa = - alfa;

					uartWriteByte(UART_USB, alfa);

					if (((anguloAct + alfa) >= 0) && ((anguloAct + alfa) <= 180))  anguloAct += alfa;

					servoWrite(SERVO_N, anguloAct);

					gpioToggle(LED3);
                }
                stringReceived -= 1;
            }
        }
    }
}

void initializeUART()
{
    uartConfig(UART_USB, 9600);   // Initialize UART_USB
    uartConfig(UART_232, 115200); // Initialize UART_232
    // Seteo un callback al evento de recepcion y habilito su interrupcion
    uartCallbackSet(UART_232, UART_RECEIVE, onRx, NULL);
    // Habilito todas las interrupciones de UART_USB
    uartInterrupt(UART_232, true);
}

bool_t initializeServo()
{
    bool_t valor = 0;
    uint8_t servoAngle = 0; // 0 to 180 degrees
    valor = servoConfig(0, SERVO_ENABLE);
    valor = servoConfig(SERVO_N, SERVO_ENABLE_OUTPUT);
    return valor;
}

void BTN_config()
{
    gpioConfig(BTN_START, GPIO_INPUT_PULLUP);
    gpioConfig(BTN_STOP, GPIO_INPUT_PULLUP);
    gpioConfig(BTN_RESET, GPIO_INPUT_PULLUP);
}

#define TRESHOLD 5
#define MAP(x) (x == BTN_START) ? 0 : (x == BTN_STOP ? 1 : 2)
bool readButtonBounce(gpioMap_t gpio){
	static bool lastState[] = {false, false, false};
	static bool lastValidState[] = {false, false, false};
	static uint8_t count[] = {0,0,0};
	uint8_t currentValue = gpioRead(gpio);
	if (currentValue == lastState[MAP(gpio)]){
		count[MAP(gpio)]++;
		if (count[MAP(gpio)] > TRESHOLD){
			lastValidState[MAP(gpio)] = currentValue;
		}
	}
	else
	{
		count[MAP(gpio)] = 0;
	}
	lastState[MAP(gpio)] = currentValue;
	return lastValidState[MAP(gpio)];
}


