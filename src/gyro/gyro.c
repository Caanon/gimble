#include "twi/twi.h"
#include "gyro/gyro.h"

// This is just for debugging purposes
#include "uart/uart.h"

unsigned char Read8(const unsigned char register_address) {
  unsigned char data = 0;

  // Set the register, then read 1 byte.
  StartI2C();
  WriteAddressI2C(GYRO_ADDRESS);
  WriteAckI2C(register_address);
  StartI2C();
  ReadAddressI2C(GYRO_ADDRESS);
  ReadNackI2C(&data);
  StopI2C();
  return data;
}

void Write8(const unsigned char register_address, const unsigned char data) {
  // Set the register, then write one byte.
  StartI2C();
  WriteAddressI2C(GYRO_ADDRESS);
  WriteAckI2C(register_address);
  WriteAckI2C(data);
  StopI2C();
}

#define GYRO_DUMP_REGISTER(name) printf("%s:\t0x%02X\n", #name, Read8(name));
void Gyro_DumpRegisters() {
  GYRO_DUMP_REGISTER(GYRO_WHO_AM_I);
  GYRO_DUMP_REGISTER(GYRO_CTRL1);
  GYRO_DUMP_REGISTER(GYRO_CTRL2);
  GYRO_DUMP_REGISTER(GYRO_CTRL3);
  GYRO_DUMP_REGISTER(GYRO_CTRL4);
  GYRO_DUMP_REGISTER(GYRO_CTRL5);
  GYRO_DUMP_REGISTER(GYRO_REFERENCE);
  GYRO_DUMP_REGISTER(GYRO_OUT_TEMP);
  GYRO_DUMP_REGISTER(GYRO_STATUS);
  GYRO_DUMP_REGISTER(GYRO_OUT_X_L);
  GYRO_DUMP_REGISTER(GYRO_OUT_X_H);
  GYRO_DUMP_REGISTER(GYRO_OUT_Y_L);
  GYRO_DUMP_REGISTER(GYRO_OUT_Y_H);
  GYRO_DUMP_REGISTER(GYRO_OUT_Z_L);
  GYRO_DUMP_REGISTER(GYRO_OUT_Z_H);
  GYRO_DUMP_REGISTER(GYRO_FIFO_CTRL);
  GYRO_DUMP_REGISTER(GYRO_FIFO_SRC);
  GYRO_DUMP_REGISTER(GYRO_IG_CFG);
  GYRO_DUMP_REGISTER(GYRO_IG_SRC);
  GYRO_DUMP_REGISTER(GYRO_IG_THIS_XH);
  GYRO_DUMP_REGISTER(GYRO_IG_THIS_XL);
  GYRO_DUMP_REGISTER(GYRO_IG_THIS_YH);
  GYRO_DUMP_REGISTER(GYRO_IG_THIS_YL);
  GYRO_DUMP_REGISTER(GYRO_IG_THIS_ZH);
  GYRO_DUMP_REGISTER(GYRO_IG_THIS_ZL);
  GYRO_DUMP_REGISTER(GYRO_IG_DURATION);
  GYRO_DUMP_REGISTER(GYRO_LOW_ODR);
}

void Gyro_SetDps(const unsigned char dps) {
  unsigned char register_value;
  if (dps > 3) {
    return;
  }
  // DPS bits are bits 4 & 5 of CTRL4;
  register_value = Read8(GYRO_CTRL4);
  register_value = (register_value & ~(3 << 4)) | (dps << 4);
  Write8(GYRO_CTRL4, register_value);
}

void Gyro_Init() {
  unsigned char data;
  data = Read8(GYRO_WHO_AM_I);
  if (data >> 1 != 0x6B) {
    printf("Unexpected device: 0x%X (%i)\n", data, data);
  } else {
    printf("Found gyro.\n");
  }

  Write8(GYRO_CTRL1, 0x0F); // Powers up the chip.

  Gyro_SetDps(GYRO_250_DPS);
}

void Gyro_ReadRaw(int *x, int *y, int *z) {
  *x = (Read8(GYRO_OUT_X_H) << 8) + Read8(GYRO_OUT_X_L);
  *y = (Read8(GYRO_OUT_Y_H) << 8) + Read8(GYRO_OUT_Y_L);
  *z = (Read8(GYRO_OUT_Z_H) << 8) + Read8(GYRO_OUT_Z_L);
}

void Gyro_ReadDegrees(float *x, float *y, float *z) {
  int x, y, z;
  Gyro_ReadRaw(&x, &y, &z);
}
