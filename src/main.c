#include <string.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "gyro/gyro.h"
#include "twi/twi.h"
#include "uart/uart.h"

#define LED2 PINA1

#define LEDPORT PORTA

#define GYRO_ADDR 0x6B
#define WHOAMI 0x0F

void DisplayMenu() {
  printf(PSTR("Menu:\n"));
  printf(PSTR("1) Poll gyro\n"));
  printf(PSTR("2) scan I2C bus\n"));
  printf(PSTR("> "));
}

void ScanI2CBus() {
  printf(PSTR("Scanning I2C bus...\n"));
  unsigned char devices[16];
  unsigned char num_devices = 0;
  I2C_ScanBus(devices, &num_devices, 16);
  for (int i = 0; i < num_devices; ++i) {
    printf(PSTR("  Found device at address 0x%X\n"), devices[i]);
  }
}

int main(void) {
  DDRA = 0b11111111;
  LEDPORT = 0b00000000;

  LEDPORT |= 1 << LED2;

  char command_buffer[16];

  InitUart();

  printf(PSTR("Initializing I2C...\n"));
  SetupI2C();

  printf_P(PSTR("Initializing Gyro...\n"));
  Gyro_Init();

  while (1) {
    DisplayMenu();
    UartGetString(command_buffer, 16);
    StreamPutChar('\n', NULL);
    printf_P(PSTR("Echo cmd: %s\n"), command_buffer);
  }

  return 0;
}
