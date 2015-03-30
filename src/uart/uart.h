#ifndef __UART_UART_H__
#define __UART_UART_H__

#include <stdio.h>

void InitUart(void);

// Returns 1 if there is data waiting to be read on the uart, otherwise 0.
// Useful for fast key press detection.
unsigned char IsDataWaiting();

char BlockingReadChar();
void BlockingWriteChar(char c);
void BlockingWriteNL();
void BlockingWriteString(const char *c);
void BlockingWriteProgmemString(const char *c);

// Functions that support printf.
int StreamPutChar(char c, FILE *file);
int StreamGetChar(FILE *file);

// String input. Returns 1 if error or escape hit. Returns 0 for success.
unsigned char UartGetString(char *buffer, unsigned int buffer_length);

// Auto-add a carriage return during printf statements. Very useful.
#define UART_ADD_CARRIAGE_RETURN

#endif // __UART_UART_H__
