/*
 * buffer.h
 *
 *  Created on: 26/10/2023
 *      Author: Barcala
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "stdint.h"



char bufferPeek();

char bufferPop();

void bufferPush(char);

uint8_t bufferIsFull();

uint8_t bufferIsEmpty();

#endif /* BUFFER_H_ */
