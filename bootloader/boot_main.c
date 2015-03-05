#include <inttypes.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>



/*
 * define CPU frequency in Mhz here if not defined in Makefile
 */
#ifndef F_CPU
  #define F_CPU 16000000UL
#endif

#define        BOOTLOADER_BLINK_LOOP_COUNT     (F_CPU / 2250)

#define LEDPORT PORTA
#define LED2 PORTA1
#define LED3 PORTA0

void (*MainProgram)(void) = 0x0;

int main(void) {

  unsigned long loop_count = 0;
  unsigned char num_blink = 0;
  
  // Set PORTA's pins to be outputs on 0 and 1.
  DDRA = _BV(LED2) | _BV(LED3);

  while(1) {
    ++loop_count;
    if (loop_count % BOOTLOADER_BLINK_LOOP_COUNT == 0) {
      LEDPORT ^= _BV(LED3);
      ++num_blink;
    }
    if (num_blink == 8) {
      break;
    }
  }
  
  // Need to make sure that EIND is set to the lower part of memory since we've above 0x1FFFF aka 64k words (128k bytes);
  EIND = 0;
  MainProgram();

  return 0; 
}
