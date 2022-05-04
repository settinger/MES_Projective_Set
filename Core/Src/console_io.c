/*
 * console_io.c
 * A wrapper between raw inputs and console operations
 * Expects console inputs to be receved on STM32F429I-DISC1 USART5
 * Ported from Elecia White's code here:
 * https://wokwi.com/projects/324879108372693587
 *
 *  Created on: May 3, 2022
 *      Author: Sam
 */

#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "usart.h"
#include "console_io.h"
//#include "circular_buffer.h"
#include "serial_logging.h"

bool uart_rx;

static uint8_t GetCharNoBlock(void) {
  uint8_t ch = '\n';
  HAL_UART_Receive_IT(&huart5, (uint8_t*) &ch, 1); // Receive one character
  if (true == uart_rx) {
    uart_rx = false;
    //HAL_UART_Receive_IT(&huart5, (uint8_t*) &ch, 1); // Receive one character

  }
  return ch;
}

consoleError ConsoleIoInit(void) {
  /* Enable UART5 interrupt*/
  HAL_NVIC_SetPriority(UART5_IRQn, 4, 0); // I have no idea what priority to assign this
  HAL_NVIC_EnableIRQ(UART5_IRQn);
  return CONSOLE_SUCCESS;
}

// Receive from console (blocking? non-blocking?)
consoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength,
    uint32_t *readLength) {
  uint32_t i = 0;
  uint8_t ch;

  HAL_UART_Receive_IT(&huart5, (uint8_t*) &ch, 1); // Receive one character
  if (uart_rx) {
    uart_rx = false;
    HAL_UART_Transmit(&huart5, (uint8_t*) &ch, 1, 250); // Display the received character in the console
    buffer[i] = ch;
    i++;
  }

//  while (huart5.RxState == HAL_UART_STATE_READY && '\r' != ch && '\n' != ch) {
//    uart_rx = false;
//    HAL_UART_Transmit(&huart5, (uint8_t*) &ch, 1, 250); // Display the received character in the console
//    buffer[i] = ch;
//    i++;
//  }

  *readLength = i;

  return CONSOLE_SUCCESS;
}

consoleError ConsoleIoSend(const char *buffer) {
  HAL_UART_Transmit(&huart5, (uint8_t*) buffer, strlen(buffer), 100);
  return CONSOLE_SUCCESS;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
  /* Set transmission flag: transfer complete*/
  //Serial_Message("Interrupt happen");
  uart_rx = true;
}
