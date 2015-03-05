#include <avr/io.h>
#include "twi_helpers.h"

#ifndef F_CPU
#define F_CPU 16000000UL  // 16MHz
#endif

/*

Status codes:

Master Transmit mode:
0x08 - Start condition has been transmitted.
0x10 - Repeated start condition has been transmitted.
0x18 - SLA+W has been transmitted, ACK has been received.
0x20 - SLA+W has been transmitted, NACK received.
0x28 - Data byte transmitted, ACK received.
0x30 - Data byte transmitted, NACK received.
0x38 - Arbitration lost in SLA+W or data bytes.

*/

void SetupI2C(void) {
  // Equation for TWBR assuming no prescalar bits is:
  // TWBR = (F_CPU / F_I2C - 16 / 2) 
  // Set up for 400kHz
  TWBR = 0xC; // aka 12

  // Make sure we set the prescalar to 0 since we don't need it.
  TWSR &= ~(1 << TWPS1 | 1 << TWPS0);


  // Apparently don't need to do this now? Only during active 
  // communication, which doesn't make sense...  

  // Set up the control register
  // Clear interrupt, enable, and enable interrupt.
  // TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
}

unsigned char StartI2C(void) {
  uint8_t twi_status;
  // Set the control register.
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

  // Note: even though we *just* set the TWCR's TWINT bit to 1, 
  // you actually "clear" the bit by writing 1 to it. In other 
  // words, actually writing a 1 to it forces it to be 0. Yeah, 
  // I don't get it either.
  // Wait until I2C "start" has been sent.
  while (!(TWCR & (1 << TWINT)) == 0) {};
 
  // Pull the status regsiter and store it. Need to store it, 
  // otherwise it might change between now and when we actually
  // read it... I think. Also mask it for just the upper 5 bits, 
  // since TWSR also stores the prescalar bits.
  twi_status = TWSR & 0xF8;
  if (twi_status != 0x08 && twi_status != 0x10 ) { 
    return 1;
  }

  return 0;
}
