#include <string.h>
#include <stdlib.h>
#include <math.h>

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
  printf_P(PSTR("e) Set gyro register\n"));
  printf_P(PSTR("f) Calibrate gyro\n"));
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
    Gyro_ReadRaw(&x, &y, &z);
    printf("%6i,%6i,%6i\n", x, y, z);
  }
  // Make sure we clear out that last keypress.
  BlockingReadChar();
}

void ByteToBitString(const unsigned char byte, char *bit_buffer,
                     unsigned char length) {
  unsigned char num_read = 0;
  for (int i = 0; i < length; ++i) {
    bit_buffer[i] = 0;
  }
  // Need - 1 so we write a 0 at the end.
  while (num_read < length - 1 && num_read < 8) {
    bit_buffer[num_read] = (byte & (1 << (7 - num_read))) ? '1' : '0';
    ++num_read;
  }
  bit_buffer[num_read] = 0;
}

unsigned char BitStringToByte(const char *bit_buffer, unsigned char length) {
  unsigned char num_read = 0;
  unsigned char value = 0;

  while (num_read < length && num_read < 8 && bit_buffer[num_read] != 0) {
    value = (value << 1) | (bit_buffer[num_read] == '1' ? 1 : 0);
    ++num_read;
  }
  return value;
}

void SetGyroRegister() {
  while (1) {
    printf_P(PSTR("Gyro Registers:\n"));
    printf_P(PSTR("1) CTRL1:\n"));
    printf_P(PSTR("2) CTRL2:\n"));
    printf_P(PSTR("3) CTRL3:\n"));
    printf_P(PSTR("4) CTRL4:\n"));
    printf_P(PSTR("5) CTRL5:\n"));
    printf_P(PSTR("> "));

    char cmd = BlockingReadChar();
    if (cmd == 27) { // Escape
      return;
    }
    BlockingWriteChar(cmd);
    BlockingWriteChar('\r');
    BlockingWriteChar('\n');

    unsigned char reg;

    switch (cmd) {
    case '1':
      reg = GYRO_CTRL1;
      printf_P(PSTR("--CTRL1--\n"));
      break;
    case '2':
      reg = GYRO_CTRL2;
      printf_P(PSTR("--CTRL2--\n"));
      break;
    case '3':
      reg = GYRO_CTRL3;
      printf_P(PSTR("--CTRL3--\n"));
      break;
    case '4':
      reg = GYRO_CTRL4;
      printf_P(PSTR("--CTRL4--\n"));
      break;
    case '5':
      reg = GYRO_CTRL5;
      printf_P(PSTR("--CTRL5--\n"));
      break;
    default:
      return;
    }

    char bits[9];
    unsigned char byte = Gyro_ReadRegister(reg);
    ByteToBitString(byte, bits, 9);
    printf_P(PSTR("Old Value: %s (0x%02X)\n"), bits, byte);
    printf_P(PSTR("Enter new value: "));
    if (UartGetString(bits, 9)) {
      printf_P(PSTR("\n"));
      continue;
    }
    printf_P(PSTR("\n"));
    byte = BitStringToByte(bits, 9);
    printf_P(PSTR("Setting new value to %s (0x%02X)\n"), bits, byte);

    Gyro_WriteRegister(reg, byte);
  }
}

#define NUM_CALIBRATION_LOOPS 1

void CalibrateGyro() {
  int *buffer = malloc(sizeof(int) * 500);
  const char spin[] = "/-\\|";
  unsigned char spin_count = 0;
  long long int cumulative;
  int vars[3];
  const char labels[] = "XYZ";

  for (int dim = 0; dim < 3; ++dim) {
    float average = 0;
    printf_P(PSTR("Calibrating %c...  "), labels[dim]);
    for (int average_count = 0; average_count < NUM_CALIBRATION_LOOPS;
         ++average_count) {
      cumulative = 0;
      for (int i = 0; i < 500; ++i) {
        Gyro_ReadNewRaw(&vars[0], &vars[1], &vars[2]);
        buffer[i] = vars[dim];
        cumulative += vars[dim];
        if (i % 100 == 0) {
          BlockingWriteChar(8);
          BlockingWriteChar(spin[spin_count]);
          ++spin_count;
          if (spin_count > 3) {
            spin_count = 0;
          }
        }
      }
      average += cumulative / 500.0f;
    }
    average /= NUM_CALIBRATION_LOOPS;
    BlockingWriteChar(8);
    printf_P(PSTR("Done. mean: %f "), average);
    float varience = 0;
    for (int i = 0; i < 500; ++i) {
      varience += (buffer[i] - average) * (buffer[i] - average);
    }
    printf_P(PSTR(" std: %f\n"), sqrt(varience / 500));
  }
  free(buffer);
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
    case 'e':
      SetGyroRegister();
      break;
    case 'f':
      CalibrateGyro();
      break;
    default:
      break;
    }
  }

  return 0;
}
