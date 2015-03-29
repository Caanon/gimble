#include <string.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "gyro/gyro.h"
#include "twi/twi.h"
#include "uart/uart.h"

#define LED2 PINA1

#define LEDPORT PORTA

void DisplayMenu() {
  printf_P(PSTR("Main menu:\n"));
  printf_P(PSTR("a) Dump gyro registers\n"));
  printf_P(PSTR("b) Init Gyro\n"));
  printf_P(PSTR("c) Scan I2C bus\n"));
  printf_P(PSTR("d) Poll gyro until key is pressed\n"));
  printf_P(PSTR("> "));
}

void ScanI2CBus() {
  printf_P(PSTR("Scanning I2C bus...\n"));
  unsigned char devices[16];
  unsigned char num_devices = 0;
  I2C_ScanBus(devices, &num_devices, 16);
  for (int i = 0; i < num_devices; ++i) {
    printf_P(PSTR("  Found device at address 0x%X\n"), devices[i]);
  }
}

void PollGyro() {
  int x, y, z;
  while (!IsDataWaiting()) {
    Gyro_Read(&x, &y, &z);
    printf("%6i,%6i,%6i\n", x, y, z);
  }
  // Make sure we clear out that last keypress.
  BlockingReadChar();
}

int main(void) {
  DDRA = 0b11111111;
  LEDPORT = 0b00000000;

  LEDPORT |= 1 << LED2;

  InitUart();

  printf(PSTR("\n\n     **RESTART**\n\n"));
  printf(PSTR("Initializing I2C...\n"));
  SetupI2C();

  while (1) {
    DisplayMenu();
    char cmd = BlockingReadChar();
    BlockingWriteChar(cmd);
    BlockingWriteChar('\r');
    BlockingWriteChar('\n');
    switch (cmd) {
    case 'a':
      Gyro_DumpRegisters();
      break;
    case 'b':
      printf_P(PSTR("Initializing Gyro...\n"));
      Gyro_Init();
      break;
    case 'c':
      ScanI2CBus();
      break;
    case 'd':
      PollGyro();
      break;
    default:
      break;
    }
  }

  return 0;
}
