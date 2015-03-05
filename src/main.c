#include <avr/io.h>

#define LED2 PINA1

#define LEDPORT PORTA

int main(void) {
  DDRA = 0b11111111;
  LEDPORT = 0b00000000;

  LEDPORT |= 1 << LED2;

  while (1) {}

  return 0;
}
