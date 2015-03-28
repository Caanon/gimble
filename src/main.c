#include <avr/io.h>
#include <avr/pgmspace.h>

#include "uart/uart.h"

#define LED2 PINA1

#define LEDPORT PORTA

const char message_progmem[] PROGMEM = "Hello progmem\r\n";
const char echo_msg[] PROGMEM = "Echoing:";

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
  BlockingWriteProgmemString(message_progmem);
  BlockingWriteChar('$');
  BlockingWriteChar('\r');
  BlockingWriteChar('\n');

  printf("This is a test: %i\r\n", 123);

  while (1) {
    char c = BlockingReadChar();
    BlockingWriteProgmemString(echo_msg);
    BlockingWriteChar(c);
    BlockingWriteNL();
  }

  return 0;
}
