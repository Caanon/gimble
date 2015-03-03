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

#define	BOOTLOADER_BLINK_LOOP_COUNT	(F_CPU / 2250)
#define MAIN_BLINK_LOOP_COUNT BOOTLOADER_BLINK_LOOP_COUNT * 4

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

#define INPORT PINC
#define WHITE PINC1
#define BLACK PINC0

#define LEDPORT PORTA

#define LED0 PINA3
#define LED1 PINA2
#define LED2 PINA1
#define LED3 PINA0

#define APP_END  (FLASHEND -(2*BOOTSIZE) + 1)

void (*application_entry_addr)(void) = 0x0000;

#define F_CPU 16000000UL
#define BAUD 115200
#define BAUD_TOL 3
#include <util/setbaud.h>


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

ISR(INT0_vect)
{
  unsigned long loop_ct = 0;
  unsigned char blink_count = 4 * 2;

  while(1) {
    ++loop_ct;
    if (loop_ct % BOOTLOADER_BLINK_LOOP_COUNT == 0) {
      LEDPORT ^= 1 << LED1;
     --blink_count;
    }
    if (blink_count == 0) {
      break;
    }
  }
}

ISR(INT1_vect)
{
  unsigned long loop_ct = 0;
  unsigned char blink_count = 4 * 2;

  while(1) {
    ++loop_ct;
    if (loop_ct % BOOTLOADER_BLINK_LOOP_COUNT == 0) {
      LEDPORT ^= 1 << LED2;
     --blink_count;
    }
    if (blink_count == 0) {
      break;
    }
  }
}

void SetMCUR(void) {
  // Set IVCE to one so we can change IVSEL
  MCUCR =  0x1;
  // Set the IVSEL bit to make sure the interrupts in the bootloader are used
  MCUCR = 0x2;
}

void InitUart(void) {
  // Set the baud rate
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

  // Status status register is UCSR0A
  // Control register is UCSR0B

  // Set 2X mode
  UCSR0A |= 1 << 1;
 
  // Need to enable transmission on control reg B, which I believe is bit 3?
  UCSR0B |= 1 << 3;

  // Set char size to 8
  UCSR0B &= ~(1 << UCSZ02); // bit 2
  UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // Bits 2, 1
 
  // Set to asynchronous mode
  UCSR0C &= ~(1 << UMSEL01 | 1 << UMSEL00); // Bits 7, 6

  // No parity
  UCSR0C &= ~(1 << UPM01 | 1 << UPM00);  // Bits 5, 4
  
  // One stop
  UCSR0C |= 1 << USBS0; // Bit 3

  // Async mode requires Clock Polarity to be rising transmit
  UCSR0C &= ~(1 << UCPOL0); // Bit 0
}

void SerialWrite(char c) {
  // set the actual 8 bits to whatever the char is
  UDR0 = c;
  // Now that we've set the usart data register, let's wait until it's transmitted. Technically, we could do other things in the meantime I guess...
  // We need to check the status register USCR0A to make sure that the transmitssion completes. Sets it to 1 when it is complete. The actual bit is TXC0 on the arduino mega 2560, which translates to bit6 of UCSR0A
  while (!(UCSR0A & (1 << 6))) {}
  // The byte has been sent off! Now we need to re-set that bit so we know what to look for next time.
  UCSR0A |= 1 << 6;
}

int main() {
  DDRA = 0b11111111;
  DDRC = 0b00000000;

  LEDPORT = 0b0000000;

  PORTC = 0b11111111;

  INPORT = 0b00000000;

  SetMCUR();

  InitUart();

  SerialWrite('S');
  SerialWrite('u');
  SerialWrite('p');
  SerialWrite('?');

  //set PD0/INT0 as input
  DDRD |= 0 << PD0;
  PORTD &= ~(1 << PD0 | 1 << PD1);

  EICRA = 0b00000000;
  EIMSK = 0b00000011 ;
  SREG |= 0b10000000;

  unsigned long loop_ct = 0;

  while (1) {
    if ((1 << WHITE) & INPORT) {
      LEDPORT |= 1 << LED0;
    } else {
      LEDPORT &= ~(1 << LED0);
    }
    
    if ((1 << BLACK) & INPORT) {
      LEDPORT |= 1 << LED1;
    } else {
      LEDPORT &= ~(1 << LED1);
    }
    ++loop_ct;
    if (loop_ct % BOOTLOADER_BLINK_LOOP_COUNT == 0) {
      LEDPORT ^= 1 << LED3;
    }
  }

  return 0;
}
