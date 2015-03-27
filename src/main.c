#include <avr/io.h>
#include <avr/pgmspace.h>

#include "uart/uart.h"

#define LED2 PINA1

#define LEDPORT PORTA

char message[] = "Hello world\r\n";
const char message_progmem[] PROGMEM = "Hello progmem\r\n";

int main(void) {
  DDRA = 0b11111111;
  LEDPORT = 0b00000000;

  LEDPORT |= 1 << LED2;

  InitUart();

  BlockingWriteChar('o');
  BlockingWriteChar('h');
  BlockingWriteChar('a');
  BlockingWriteChar('i');
  BlockingWriteChar('!');
  BlockingWriteChar('\r');
  BlockingWriteChar('\n');

  BlockingWriteString(message);
  BlockingWriteProgmemString(message_progmem);
  BlockingWriteChar('$');
  BlockingWriteChar('\r');
  BlockingWriteChar('\n');

  while (1) {}

  return 0;
}
