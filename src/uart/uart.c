#include <avr/io.h>
#include <avr/pgmspace.h>

#ifndef BAUD
#define BAUD 115200
#endif

#define BAUD_TOL 3
// Must be included AFTER the above defines.
#include <util/setbaud.h>

void InitUart(void) {
  // Set the Baud Rate. UBRRH_VALUE is pulled in from util/setbaud.h.
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

  // Status register is USCR0A.
  // Control register is UCSR0B.

  // Set 2X mode.
  UCSR0A |= _BV(U2X0);

  // Enable transmission.
  UCSR0B |= _BV(TXEN0);

  // Set char size to 8.
  UCSR0B &= ~_BV(UCSZ02);              // bit 2
  UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01); // Set bits 1,2

  // Set to asynchronous mode.
  UCSR0C &= ~(_BV(UMSEL00) | _BV(UMSEL01)); // Clear bits 6,7

  // No parity
  UCSR0C |= _BV(USBS0); // Set bit 3.

  // Async mode requires Clock Polarity to be rising transmit.
  UCSR0C &= ~_BV(UCPOL0); // Clear bit 0.
}

// Blocking serial write. Loops until the transmission is clear.
void BlockingWriteChar(char c) {
  // Load the char to send.
  UDR0 = c;

  // Now that we've set the usart data register, let's wait until it's
  // transmitted. Technically, we could do other things in the meantime I
  // guess...

  // We need to check the status register USCR0A to make sure that the
  // transmitssion completes. Sets it to 1 when it is complete. The actual bit
  // is TXC0 on the arduino mega 2560, which translates to bit6 of UCSR0A

  while (!(UCSR0A & (1 << 6))) {
  }

  // The byte has been sent off! Now we need to re-set that bit so we know what
  // to look for next time.
  UCSR0A |= 1 << 6;
}

// Writes until zero is hit.
void BlockingWriteString(char *c) {
  while (*c != 0) {
    BlockingWriteChar(*c);
    ++c;
  }
}

// Writes progmem until zero is hit.
void BlockingWriteProgmemString(const void *c) {
  char output;
  do {
    // Don't really need "far" here, since .data isn't that high in memory.
    // Couldn't hurt though.
    output = pgm_read_byte_far((uint16_t)c);
    if (output != 0) {
      BlockingWriteChar(output);
    }
    ++c;
  } while (output != 0);
}
