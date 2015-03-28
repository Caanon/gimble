#include <avr/io.h>
#include <avr/pgmspace.h>

#include "twi/twi.h"
#include "uart/uart.h"

#define LED2 PINA1

#define LEDPORT PORTA

const char message_progmem[] PROGMEM = "Hello progmem\r\n";
const char echo_msg[] PROGMEM = "Echoing:";

#define GYRO_ADDR 0x6B

#define WHOAMI 0x0F

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

  SetupI2C();

  unsigned char error_status = 0;

  printf("Press key to run I2c...\r\n");
  while (1) {
    char c = BlockingReadChar();
    printf("Running command: %c (ascii: %i)\r\n", c, c);
    BlockingWriteNL();

    // Start off the I2C
    printf("Start\r\n");
    error_status = StartI2C();
    if (error_status) {
      printf("--Error Start: %x\r\n", error_status);
    }

    // Write the address of the gyro.
    printf("SLA+W\r\n");
    error_status = WriteAddressI2C(GYRO_ADDR);
    if (error_status) {
      printf("--Error SLA+W: %x\r\n", error_status);
    }

    // Wrijte the sub-address that we're going to read from.
    printf("SUB\r\n");
    error_status = WriteAckI2C(WHOAMI);
    if (error_status) {
      printf("--Error WACK: %x\r\n", error_status);
    }

    // Start again, but this time in read mode.
    printf("RStart\r\n");
    error_status = StartI2C();
    if (error_status) {
      printf("--Error Repeated Start: %x\r\n", error_status);
    }

    // Set SLA+W mode.
    printf("SLA+R\r\n");
    error_status = ReadAddressI2C(GYRO_ADDR);
    if (error_status) {
      printf("--Error SLA+R: %x\r\n", error_status);
    }

    // See if we can get data...
    printf("RACK\r\n");
    unsigned char data = 3;
    error_status = ReadNackI2C(&data);
    if (error_status) {
      printf("--Error RNACK: %x\r\n", error_status);
    } else {
      printf("Somehow managed to get data over I2C: %x (%i)\r\n", data, data);
    }

    // Finally make stop condition.
    printf("Stop\r\n");
    StopI2C();

    printf("Press key to try again...\r\n");
  }

  return 0;
}
