#ifndef _TWI_TWI_H_
#define _TWI_TWI_H_

// Enable TWI registers.
void SetupI2C(void);

// Send START command.
unsigned char StartI2C();

// Set the address of the thing we're going to talk to, aka setting the SLA+W.
unsigned char WriteAddressI2C(const unsigned char address);

// Write a byte to I2C. Must have started and set I2C address.
unsigned char WriteAckI2C(const unsigned char byte);
unsigned char WriteNackI2C(const unsigned char byte);

// Set the address of the thing we're going to talk to, aka setting the SLA+R.
unsigned char ReadAddressI2C(const unsigned char address);
// Read a byte fron I2C. Must have started and set I2C address. Expect more
// data.
unsigned char ReadAckI2C(unsigned char *byte);
// Read a byte fron I2C. Must have started and set I2C address. Expect no more
// data.
unsigned char ReadNackI2C(unsigned char *byte);

// Stop I2C communication in master mode.
void StopI2C();

// Scan the address range from 0-127, retrieving up to 'limi' values. Does so
// by sending SLA+W to each address and listening for an ACK. If the request
// fails, it stops sending and continues to the next address. If it succeeds, it
// notes the address and continues.
void I2C_ScanBus(unsigned char *data, unsigned char *num_devices,
                 const unsigned char limit);

#endif
