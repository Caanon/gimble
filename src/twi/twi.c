#include <avr/io.h>
#include "twi.h"

#ifndef F_CPU
#define F_CPU 16000000UL // 16MHz
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

Master Receive Mode:
0x40 - SLA+R transmitted, ACK received.
0x48 - SLA+R transmitted, NACK received.
0x50 - DATA + ACK
0x58 - DATA + NACK

*/

void SetupI2C(void) {
  // Equation for TWBR assuming no prescalar bits is:
  // TWBR = (F_CPU / F_I2C - 16 / 2)
  // Set up for 400kHz
  TWBR = 0xC; // aka 12

  // Make sure we set the prescalar to 0 since we don't need it.
  TWSR &= ~(_BV(TWPS1) | _BV(TWPS0));
}

unsigned char StartI2C(void) {
  uint8_t twi_status;
  // Set the control register.
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

  // Note: even though we *just* set the TWCR's TWINT bit to 1,
  // you actually "clear" the bit by writing 1 to it. In other
  // words, actually writing a 1 to it forces it to be 0. Yeah,
  // I don't get it either.
  // Wait until I2C "start" has been sent.
  while (!(TWCR & (1 << TWINT))) {
  };

  // Pull the status regsiter and store it. Need to store it,
  // otherwise it might change between now and when we actually
  // read it... I think. Also mask it for just the upper 5 bits,
  // since TWSR also stores the prescalar bits.
  twi_status = TWSR & 0xF8;

  // Check for start or repeated start.
  if (twi_status != 0x08 && twi_status != 0x10) {
    return twi_status;
  }

  return 0;
}

unsigned char WriteAddressI2C(const unsigned char address) {
  uint8_t twi_status;

  // Load the address into the two-wire data register.
  TWDR = address << 1; // need last bit to be 0 for write mode.

  // Clear the TWI bit, making sure enable was still hit.
  TWCR = _BV(TWINT) | _BV(TWEN);

  // Wait until SLA+W is sent
  while (!(TWCR & (1 << TWINT))) {
  };

  // Check the result
  twi_status = TWSR & 0xF8;
  if (twi_status != 0x18) {
    // TODO(blakely): Might need to check other conditions here, like
    // arbitration lost?
    return twi_status;
  }

  return 0;
}

unsigned char WriteAckI2C(const unsigned char byte) {
  uint8_t twi_status;
  TWDR = byte;
  // Clear the TWI bit, making sure enable was still set.
  TWCR = _BV(TWINT) | _BV(TWEN);

  // Wait until the data byte is sent
  while (!(TWCR & (1 << TWINT))) {
  };

  // Check the result
  twi_status = TWSR & 0xF8;
  if (twi_status != 0x28) {
    // Weird.
    return twi_status;
  }

  return 0;
}

unsigned char WriteNackI2C(const unsigned char byte) {
  uint8_t twi_status;

  TWDR = byte;
  // Clear the TWI bit, making sure enable was still set.
  TWCR = _BV(TWINT) | _BV(TWEN);

  // Wait until the data byte is sent
  while (!(TWCR & (1 << TWINT))) {
  };

  // Check the result
  twi_status = TWSR & 0xF8;
  if (twi_status != 0x30) {
    // Weird.
    return twi_status;
  }

  return 0;
}

// READ

unsigned char ReadAddressI2C(const unsigned char address) {
  uint8_t twi_status;

  // Load the address into the two-wire data register.
  TWDR = address << 1 | 0x1; // need last bit to be 1 for read mode.

  // Clear the TWI bit, making sure enable was still hit.
  TWCR = _BV(TWINT) | _BV(TWEN);

  // Wait until SLA+R is sent
  while (!(TWCR & (1 << TWINT))) {
  };

  // Check the result
  twi_status = TWSR & 0xF8;
  if (twi_status != 0x40) {
    return twi_status;
  }

  return 0;
}

unsigned char ReadAckI2C(unsigned char *byte) {
  uint8_t twi_status;
  // Clear TWI bit, making sure enable was still set.
  TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);

  // Wait until there's data.
  while (!(TWCR & (1 << TWINT))) {
  };

  // Save the data.
  *byte = TWDR;

  // Check the result
  twi_status = TWSR & 0xF8;
  if (twi_status != 0x50) {
    return twi_status;
  }
  return 0;
}

unsigned char ReadNackI2C(unsigned char *byte) {
  uint8_t twi_status;
  // Clear TWI bit, making sure enable was still set.
  TWCR = _BV(TWINT) | _BV(TWEN);

  // Wait until there's data.
  while (!(TWCR & (1 << TWINT))) {
  };

  // Save the data.
  *byte = TWDR;

  // Check the result
  twi_status = TWSR & 0xF8;
  if (twi_status != 0x58) {
    return twi_status;
  }
  return 0;
}

void StopI2C() {
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
  // Don't need to check that TWINT was set here, since stop conditions don't
  // set it.
}

void I2C_ScanBus(unsigned char *data, unsigned char *num_devices,
                 const unsigned char limit) {
  SetupI2C();
  *num_devices = 0;
  unsigned char address = 0;
  while (*num_devices < limit && address < 127) {
    StartI2C();
    if (!WriteAddressI2C(address)) {
      // Device acknowledged.
      data[*num_devices] = address;
      ++(*num_devices);
    }
    StopI2C();
    ++address;
  }
}
