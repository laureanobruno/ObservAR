/*
 * buffer.c
 *
 *  Created on: 26/10/2023
 *      Author: Barcala
 */

#include "buffer.h"

#ifndef DIMF // Si el usuario lo define antes tiene otro tama�o
#define DIMF 200
#endif

char buffer[DIMF];
uint8_t pWrite = 0;
uint8_t pRead = 0;

char bufferPeek()
{
	if (!bufferIsEmpty())
	{
		return buffer[pRead];
	}
	else
	{
		return 0;
	}
}

char bufferPop()
{
	if (!bufferIsEmpty())
	{
		char aux = buffer[pRead];
		pRead = (pRead + 1) % DIMF;
		return aux;
	}
	else
		return 0;
}

void bufferPush(char c)
{
	if (!bufferIsFull())
	{
		buffer[pWrite] = c;
		pWrite = (pWrite + 1) % DIMF;
	}
}

uint8_t bufferIsFull()
{
	return (((pWrite + 1) % DIMF) == pRead);
}

uint8_t bufferIsEmpty()
{
	return (pRead == pWrite);
}
