#ifndef __UART_UART_H__
#define __UART_UART_H__

#include <stdio.h>

void InitUart(void);

char BlockingReadChar();
void BlockingWriteChar(char c);
void BlockingWriteNL();
void BlockingWriteString(const char* c);
void BlockingWriteProgmemString(const char* c);

// Functions that support printf.
int StreamPutChar(char c, FILE* file);
int StreamGetChar(FILE* file);


#endif  // __UART_UART_H__
