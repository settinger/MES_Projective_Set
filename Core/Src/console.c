/*
 * console.c
 * CLI code ported from Elecia White's:
 * https://wokwi.com/projects/324879108372693587
 *
 *  Created on: May 3, 2022
 *      Author: Sam
 */

#include <string.h>  // for NULL
#include <stdlib.h>  // for atoi and itoa (though this code implement a version of that)
#include <stdbool.h>
#include "console.h"
#include "console_io.h"
#include "console_commands.h"
#include "serial_logging.h"

#ifndef MIN
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif

#define NOT_FOUND -1
#define INT16_MAX_STRLEN 8 // -65534: six characters plus two NULL
#define INT32_MAX_STRLEN 16
#define NULL_CHAR '\0'
#define CR_CHAR '\r'
#define LF_CHAR '\n'

// global variables
char receiveBuffer[CONSOLE_COMMAND_MAX_LENGTH];
uint32_t receivedSoFar;
bool receiveBufferNeedsChecking = false;

// local functions
static int32_t ConsoleCommandEndline(const char receiveBuffer[],
    const uint32_t filledLength);
static uint32_t ConsoleCommandMatch(const char *name, const char *buffer);
static commandResult ConsoleParamFindN(const char *buffer,
    const uint8_t parameterNumber, uint32_t *startLocation);
static uint32_t ConsoleResetBuffer(char receiveBuffer[],
    const uint32_t filledLength, uint32_t usedSoFar);

static commandResult ConsoleUtilHexCharToInt(char charVal, uint8_t *pInt); // might be replaced with *pInt = atoi(str)
static commandResult ConsoleUtilIntToHexChar(uint8_t intVal, char *pChar); // might be replaced with itoa(intVal, str, 16)

// ConsoleCommandMatch
// Look to see if the data in the buffer matches the command name given that
// the strings are different lengths and we have parameter separators
static uint32_t ConsoleCommandMatch(const char *name, const char *buffer) {
  uint32_t i = 0u;
  uint32_t result = 0u;

  if (buffer[i] == name[i]) {
    result = 1u;
    i++;
  }

  while ((1u == result) && (i < CONSOLE_COMMAND_MAX_COMMAND_LENGTH)
      && (buffer[i] != PARAMETER_SEPARATOR) && (buffer[i] != LF_CHAR)
      && (buffer[i] != CR_CHAR) && (buffer[i] != (char) NULL_CHAR)) {
    if (buffer[i] != name[i]) {
      result = 0u;
    }
    i++;
  }
  return result;
}

/*
 * ConsoleResetBuffer
 * In an ideal world, this would just zero out the buffer.
 * However, there are times when the buffer may have data
 * beyond what was used in the last command. We don't want
 * to lose that data so we move it to the start of the
 * command buffer and then zero the rest.
 */

static uint32_t ConsoleResetBuffer(char receiveBuffer[],
    const uint32_t filledLength, uint32_t usedSoFar) {
  uint32_t remaining = (filledLength - usedSoFar);
  uint32_t i = 0;

  while (usedSoFar < filledLength) {
    receiveBuffer[i] = receiveBuffer[usedSoFar]; // Move the end to the start
    i++;
    usedSoFar++;
  }
  // Fill the rest of the buffer with NULL characters
  for ( /* nothing */; i < CONSOLE_COMMAND_MAX_LENGTH; i++) {
    receiveBuffer[i] = NULL_CHAR;
  }
  return remaining;
}

/* ConsoleCommandEndline
 * Check to see where in the buffer stream the endline is;
 * that is the end of the command and parameters.
 */
static int32_t ConsoleCommandEndline(const char receiveBuffer[],
    const uint32_t filledLength) {
  uint32_t i = 0;
  int32_t result = NOT_FOUND; // If no endline, return -1 (NOT_FOUND)

  while ((CR_CHAR != receiveBuffer[i]) && (LF_CHAR != receiveBuffer[i])
      && (i < filledLength)) {
    i++;
  }
  if (i < filledLength) {
    result = i;
  }
  return result;
}

/*
 * ConsoleInit
 * Initialize console interface and what it depends on.
 */
void ConsoleInit(void) {
  uint32_t i;
  ConsoleIoInit();
  Serial_Message("teste");
  ConsoleIoSend("Console online.");
  ConsoleIoSend(ENDLINE);
  ConsoleIoSend(CONSOLE_PROMPT);
  receivedSoFar = 0u;

  for (i = 0u; i < CONSOLE_COMMAND_MAX_LENGTH; i++) {
    receiveBuffer[i] = NULL_CHAR;
  }
}

/*
 * ConsoleProcess
 * Look for new inputs that have arrived; check for endline; then run matching command.
 * Call this from a loop to handle commands as they become available.
 */
void ConsoleProcess(void) {
  const consoleCommandTable* commandTable;
  uint32_t received;
  uint32_t commandIndex;
  int32_t commandEndline;
  int32_t found;
  commandResult result;

  ConsoleIoReceive((uint8_t*) &(receiveBuffer[receivedSoFar]),
      (CONSOLE_COMMAND_MAX_LENGTH - receivedSoFar), &received);

  if (received > 0 || receiveBufferNeedsChecking) {
    receiveBufferNeedsChecking = false;
    receivedSoFar += received;
    commandEndline = ConsoleCommandEndline(receiveBuffer, receivedSoFar);
    if (commandEndline >= 0) { // Received a complete string with a command
      ConsoleIoSend(ENDLINE);
      // Check if the received message is a valid instruction
      commandTable = ConsoleCommandsGetTable();
      commandIndex = 0u;
      found = NOT_FOUND;
      while ((NULL != commandTable[commandIndex].name) && (NOT_FOUND == found)) {
        if (ConsoleCommandMatch(commandTable[commandIndex].name,
            receiveBuffer)) {
          result = commandTable[commandIndex].execute(receiveBuffer);
          if (COMMAND_SUCCESS != result) {
            ConsoleIoSend("ERROR: ");
            ConsoleIoSend(receiveBuffer);
            ConsoleIoSend("Help: ");
            ConsoleIoSend(commandTable[commandIndex].help);
            ConsoleIoSend(ENDLINE);
          }
          found = commandIndex;
        } else {
          commandIndex++;
        }
      }

      if ((commandEndline != 0) && (NOT_FOUND == found)) {
        if (receivedSoFar > 2) { // ignore short commands
          ConsoleIoSend("Command not found.");
          ConsoleIoSend(ENDLINE);
        }
      }

      // Reset buffer by moving any leftovers and nulling the rest.
      // This clears up to and including the front endline character.
      receivedSoFar = ConsoleResetBuffer(receiveBuffer, receivedSoFar,
          commandEndline + 1);
      receiveBufferNeedsChecking = (receivedSoFar > 0 ? true : false);
      ConsoleIoSend(CONSOLE_PROMPT);
    }
  }
}

/* ConsoleParamFindN
 * Find the start location of the nth parameter in the buffer
 * where the command itself is parameter 0
 */
static commandResult ConsoleParamFindN(const char *buffer,
    const uint8_t parameterNumber, uint32_t *startLocation) {
  uint32_t bufferIndex = 0;
  uint32_t parameterIndex = 0;
  commandResult result = COMMAND_SUCCESS;

  while ((parameterNumber != parameterIndex)
      && (bufferIndex < CONSOLE_COMMAND_MAX_LENGTH)) {
    if (PARAMETER_SEPARATOR == buffer[bufferIndex]) {
      parameterIndex++;
    }
    bufferIndex++;
  }
  if (CONSOLE_COMMAND_MAX_LENGTH == bufferIndex) {
    result = COMMAND_PARAMETER_ERROR;
  } else {
    *startLocation = bufferIndex;
  }
  return result;
}

/* ConsoleReceive_int16
 * Identify and obtain a parameter of type int16_t, sent in in decimal, possibly with a negative sign.
 * Note that this uses atoi, a somewhat costly function. You may want to replace it.
 * See ConsoleReceiveHex_uint16 for some ideas on how to do that.
 */
commandResult ConsoleReceive_int16(const char *buffer,
    const uint8_t parameterNumber, int16_t *parameterInt) {
  uint32_t startIndex = 0;
  uint32_t i;
  commandResult result;
  char charVal;
  char str[INT16_MAX_STRLEN];

  result = ConsoleParamFindN(buffer, parameterNumber, &startIndex);

  i = 0;
  charVal = buffer[startIndex + i];
  while ((LF_CHAR != charVal) && (CR_CHAR != charVal)
      && (PARAMETER_SEPARATOR != charVal) && (i < INT16_MAX_STRLEN)) {
    str[i] = charVal; // Copy the relevant part
    i++;
    charVal = buffer[startIndex + i];
  }

  if (i == INT16_MAX_STRLEN) {
    result = COMMAND_PARAMETER_ERROR;
  }

  if (COMMAND_SUCCESS == result) {
    str[i] = NULL_CHAR;
    *parameterInt = atoi(str);
  }

  return result;
}

/* ConsoleReceiveHex_uint16
 * Identify and obtain a parameter of type uint16, sent in as hex.
 * This parses the number and does not use a library function to do so.
 */
commandResult ConsoleReceiveHex_uint16(const char *buffer,
    const uint8_t parameterNumber, uint16_t *parameterUint16) {
  uint32_t startIndex = 0;
  uint16_t value = 0;
  uint32_t i;
  commandResult result;
  uint8_t tmpuint8;

  result = ConsoleParamFindN(buffer, parameterNumber, &startIndex);
  if (COMMAND_SUCCESS == result) {
    // bufferIndex points to start of string to be parsed as integer
    // Next separator or newline or NULL indicates end of it
    for (i = 0u; i < 4u; i++) { // uint16 must be 4 or fewer hex digits
      if (COMMAND_SUCCESS == result) {
        result = ConsoleUtilHexCharToInt(buffer[startIndex + i], &tmpuint8);
      }
      if (COMMAND_SUCCESS == result) {
        value = value << 4u;
        value += tmpuint8;
      }
    }
    if (COMMAND_PARAMETER_END == result) {
      result = COMMAND_SUCCESS;
    }
    *parameterUint16 = value;
  }
  return result;
}

/* ConsoleSendHex_uint16
 * Send a parameter of type uint16_t as hex-formatted string
 * This does not use a library function to do it; however, you
 * could use itoa(paramuint16, out, 16) instead.
 */

commandResult ConsoleSendHex_uint16(uint16_t parameterUint16) {
  uint32_t i;
  char out[4u + 1u]; // A uint16 parameter must be 4 or fewer hex digits, add extra unit for NULL termination
  commandResult result = COMMAND_SUCCESS;
  uint8_t tmpuint8;

  for (i = 0u; i < 4u; i++) {
    if (COMMAND_SUCCESS == result) {
      tmpuint8 = (parameterUint16 >> (12u - (i * 4u)) & 0xF);
      result = ConsoleUtilIntToHexChar(tmpuint8, &(out[i]));
    }
  }
  out[i] = NULL_CHAR;
  ConsoleIoSend(out);

  return COMMAND_SUCCESS;
}

/* ConsoleSendHex_uint8
 * Very similar to previous function.
 */
commandResult ConsoleSendHex_uint8(uint8_t parameterUint8) {
  uint32_t i;
  char out[2u + 1u]; //uint8 parameter is 2 hex digits, allow for a null terminator too
  commandResult result = COMMAND_SUCCESS;

  i = 0u;
  result = ConsoleUtilIntToHexChar((parameterUint8 >> 4u) & 0xF, &(out[i]));
  i++;
  if (COMMAND_SUCCESS == result) {
    result = ConsoleUtilIntToHexChar(parameterUint8 & 0xF, &(out[i]));
    i++;
  }
  out[i] = NULL_CHAR;
  ConsoleIoSend(out);

  return COMMAND_SUCCESS;
}

/*
 * The C library itoa is sometimes a complicated function and the library costs
 * aren't worth it, so this implements the necessary parts for console.
 */
#if CONSOLE_USE_BUILTIN_ITOA
#define itoa smallItoa
static void smallItoa(int in, char *outBuffer, int radix) {
  bool isNegative = false;
  int tmpIn;
  int stringLen = 1u;  // String will be at least as long as the NULL character

  if (in < 0) {
    isNegative = true;
    in = -in;
    stringLen++;
  }

  // Get length of string to be composed
  tmpIn = in;
  while ((int) tmpIn / radix != 0) {
    tmpIn = (int) tmpIn / radix;
    stringLen++;
  }

  // Starting with NULL character, fill in string backwards
  *(outBuffer + stringLen) = NULL_CHAR;
  stringLen--;

  tmpIn = in;
  do {
    *(outBuffer + stringLen) = (tmpIn % radix) + '0';
    tmpIn = (int) (tmpIn / radix);
  } while (stringLen--);

  if (isNegative) {
    *(outBuffer) = '-';
  }
}
#endif

/* ConsoleSend_int16
 * Send a parameter of type int16t using the (unsafe) C library function
 * itoa to translate from integer to string.
 */
commandResult ConsoleSend_int16(int16_t parameterInt) {
  char out[INT16_MAX_STRLEN];
  // memset(out, 0, INT16_MAX_STRLEN);

  itoa(parameterInt, out, 10);
  ConsoleIoSend(out);

  return COMMAND_SUCCESS;
}

/* ConsoleSend_int32
 * Send a parameter of type int32t using the (unsafe) C library function
 * itoa to translate from integer to string.
 */
commandResult ConsoleSend_int32(int32_t parameterInt) {
  char out[INT32_MAX_STRLEN];
  memset(out, 0, sizeof(out));

  itoa(parameterInt, out, 10);
  ConsoleIoSend(out);

  return COMMAND_SUCCESS;
}

/* ConsoleUtilHexCharToInt
 * Convert a single hex character (0-9,A-F) to integer (0-15)
 */
static commandResult ConsoleUtilHexCharToInt(char charVal, uint8_t *pInt) {
  commandResult result = COMMAND_SUCCESS;

  if (('0' <= charVal) && (charVal <= '9')) {
    *pInt = charVal - '0';
  } else if (('A' <= charVal) && (charVal <= 'F')) {
    *pInt = 10u + charVal - 'A';
  } else if (('a' <= charVal) && (charVal <= 'f')) {
    *pInt = 10u + charVal - 'a';
  } else if ((LF_CHAR != charVal) || (CR_CHAR != charVal)
      || (PARAMETER_SEPARATOR == charVal)) {
    result = COMMAND_PARAMETER_END;
  } else {
    result = COMMAND_PARAMETER_ERROR;
  }
  return result;
}

/* ConsoleUtilIntToHexChar
 * Convert an integer nibble (0-15) to a hex character 0x0-0xF
 */
static commandResult ConsoleUtilIntToHexChar(uint8_t intVal, char *pChar) {
  commandResult result = COMMAND_SUCCESS;

  if (intVal <= 9u) {
    *pChar = intVal + '0';
  } else if ((10u <= intVal) && (intVal <= 15u)) {
    *pChar = intVal - 10u + 'A';
  } else {
    result = COMMAND_PARAMETER_ERROR;
  }
  return result;
}

/* ConsoleSendString
 * Send a null terminated string to the console.
 * This is a light wrapper around ConsoleIoSend. It uses the same
 * API convention as the rest of the top level ConsoleSendX APIs
 * while exposing this functionality at the top level so that the
 * lower level console_io module doesn't need to be a dependency.
 */
commandResult ConsoleSendString(const char * buffer) {
  ConsoleIoSend(buffer);
  return COMMAND_SUCCESS;
}

commandResult ConsoleSendLine(const char * buffer) {
  ConsoleIoSend(buffer);
  ConsoleIoSend(ENDLINE);
  return COMMAND_SUCCESS;
}
