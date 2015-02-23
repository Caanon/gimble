#include <inttypes.h>
#include <avr/io.h>

/*
 * define CPU frequency in Mhz here if not defined in Makefile
 */
#ifndef F_CPU
  #define F_CPU 16000000UL
#endif

#define	_BLINK_LOOP_COUNT_	(F_CPU / 2250)

#ifndef BAUDRATE
	#define BAUDRATE 115200
#endif

/*
 * Calculate the address where the bootloader starts from FLASHEND and BOOTSIZE
 * (adjust BOOTSIZE below and BOOTLOADER_ADDRESS in Makefile if you want to change the size of the bootloader)
 */
//#define BOOTSIZE 1024
#if FLASHEND > 0x0F000
	#define BOOTSIZE 8192
#else
	#define BOOTSIZE 2048
#endif

#define APP_END  (FLASHEND -(2*BOOTSIZE) + 1)

void (*application_entry_addr)(void) = 0x0000;

/* 
  Entry point into the bootloader. 

The AVR library usually handles .init0 through .init9 and then jumps into main. The linker puts together the program so that the bootloader address of atmega2560 (0x3F000) is set to jump to .init0 which, unless overridden, is set up by the linker. We're going to override it to set up the stack pointer, which apparently is a problem with some chips. 

We set "naked" so that there's not additional gcc code that wraps this function. This also means that we can't have anything here but assembly.
*/

void __jumpMain	(void) __attribute__ ((naked)) __attribute__ ((section (".init9")));

void __jumpMain (void) {
  // This *should* set the stack to the top of RAM.
  asm volatile ( ".set __stack, %0" :: "i" (RAMEND));

  // But in the case that it doesn't, this should set it correctly.

  // Load the high bits into temp register 16.
  asm volatile ( "ldi 16, %0" :: "i" (RAMEND >> 8));
  // Put register 16 into the high bits of the stack pointer.
  asm volatile ( "out %0, 16" :: "i" (AVR_STACK_POINTER_HI_ADDR));

  // Load the low bits into temp register 16.
  asm volatile ( "ldi 16, %0" :: "i" (RAMEND && 0x0FF));
  // Put register 16 into the low bits of the stack pointer.
  asm volatile ( "out %0, 16" :: "i" (AVR_STACK_POINTER_LO_ADDR));

  // Make sure the zero register is actually zero.
  asm volatile ( "clr __zero_reg__" );
  // And zero out the status register.
  asm volatile ( "out %0, __zero_reg__" :: "I" (_SFR_IO_ADDR(SREG)) );
  // Now that everything is set up, jump to main.
  asm volatile ( "jmp main" );
}

void main() {
}
