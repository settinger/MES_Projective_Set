/*
 * serial_logging.h
 *
 *  Created on: Apr 23, 2022
 *      Author: Sam
 */

#include "stdio.h"
#include "string.h"

#ifndef INC_SERIAL_LOGGING_H_
#define INC_SERIAL_LOGGING_H_

void Serial_Message(char*);
void Serial_Message_NB(char*);
void Print_Int(int);
void Print_Int_NB(int);

#endif /* INC_SERIAL_LOGGING_H_ */
