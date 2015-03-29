#include <avr/io.h>
#include <avr/pgmspace.h>

#include "gyro/gyro.h"
#include "twi/twi.h"
#include "uart/uart.h"

#define LED2 PINA1

#define LEDPORT PORTA

#define GYRO_ADDR 0x6B
#define WHOAMI 0x0F

int main(void) {
  DDRA = 0b11111111;
  LEDPORT = 0b00000000;

  LEDPORT |= 1 << LED2;

  InitUart();

  SetupI2C();

  printf("Scanning I2C bus...\n");
  unsigned char devices[16];
  unsigned char num_devices = 0;
  I2C_ScanBus(devices, &num_devices, 16);
  for (int i = 0; i < num_devices; ++i) {
    printf("  Found device at address 0x%X\n", devices[i]);
  }

  printf("Initializing Gyro...\n");
  Gyro_Init();

  unsigned char error_status = 0;

  printf("Press key to run I2C...\n");
  while (1) {
    char c = BlockingReadChar();
    printf("Running command: %c (ascii: %i)\n", c, c);
    BlockingWriteNL();

    // Start off the I2C
    printf("Start\n");
    error_status = StartI2C();
    if (error_status) {
      printf("--Error Start: %x\n", error_status);
    }

    // Write the address of the gyro.
    printf("SLA+W\n");
    error_status = WriteAddressI2C(GYRO_ADDR);
    if (error_status) {
      printf("--Error SLA+W: %x\n", error_status);
    }

    // Wrijte the sub-address that we're going to read from.
    printf("SUB\n");
    error_status = WriteAckI2C(WHOAMI);
    if (error_status) {
      printf("--Error WACK: %x\n", error_status);
    }

    // Start again, but this time in read mode.
    printf("RStart\n");
    error_status = StartI2C();
    if (error_status) {
      printf("--Error Repeated Start: %x\n", error_status);
    }

    // Set SLA+W mode.
    printf("SLA+R\n");
    error_status = ReadAddressI2C(GYRO_ADDR);
    if (error_status) {
      printf("--Error SLA+R: %x\n", error_status);
    }

    unsigned char data = 3;

    // See if we can get data...
    printf("RACK\n");
    error_status = ReadAckI2C(&data);
    if (error_status) {
      printf("--Error RNACK: %x\n", error_status);
    } else {
      printf("Somehow managed to get data over I2C: %x (%i)\n", data, data);
      printf("Does 0x6B == 0x%x?\n", data >> 1);
    }

    // Finally make stop condition.
    printf("Stop\n");
    StopI2C();

    printf("Press key to try again...\n");
  }

  return 0;
}
