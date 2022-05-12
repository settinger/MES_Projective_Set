/*
 * serial_logging.c
 *
 *  Created on: Apr 23, 2022
 *      Author: Sam
 */

#include "serial_logging.h"
#include "usart.h"

// Send a message over serial
void Serial_Message(char string[]) {
  HAL_UART_Transmit(&huart5, (uint8_t*) string, strlen((char*) string), 100);
  HAL_UART_Transmit(&huart5, (uint8_t*) "\r\n", 2, 100);
}

// Send a message over serial
void Serial_Message_NB(char string[]) {
  HAL_UART_Transmit(&huart5, (uint8_t*) string, strlen((char*) string), 100);
}

void Print_Int(int x) {
  char string[24]; // Lazy assume integer is fewer than 24 digits
  sprintf(string, "%d", x);
  HAL_UART_Transmit(&huart5, (uint8_t*) string, strlen(string), 100);
  HAL_UART_Transmit(&huart5, (uint8_t*) "\r\n", 2, 100);
}

void Print_Int_NB(int x) {
  char string[24]; // Lazy assume integer is fewer than 24 digits
  sprintf(string, "%d", x);
  HAL_UART_Transmit(&huart5, (uint8_t*) string, strlen(string), 100);
}
