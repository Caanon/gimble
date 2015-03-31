#include "accel/accel.h"
#include "twi/twi.h"
#include "uart/uart.h"

unsigned char Accel_Read8(const unsigned char address,
                          const unsigned char register_address) {
  unsigned char data = 0;

  // Set the register, then read 1 byte.
  StartI2C();
  WriteAddressI2C(address);
  WriteAckI2C(register_address);
  StartI2C();
  ReadAddressI2C(address);
  ReadNackI2C(&data);
  StopI2C();
  return data;
}

void Accel_Write8(const unsigned char address,
                  const unsigned char register_address,
                  const unsigned char data) {
  // Set the register, then write one byte.
  StartI2C();
  WriteAddressI2C(address);
  WriteAckI2C(register_address);
  WriteAckI2C(data);
  StopI2C();
}

void Accel_Init() {
  // Set register to 0101 0111 to enable 100Hz and enable x/y/z.
  Accel_Write8(ACCEL_ADDRESS, ACCEL_CTRL_REG1_A, 0x57);
}

void Accel_ReadRaw(int *x, int *y, int *z) {
  *x = (Accel_Read8(ACCEL_ADDRESS, ACCEL_OUT_X_H_A) << 8) +
       Accel_Read8(ACCEL_ADDRESS, ACCEL_OUT_X_L_A);
  *y = (Accel_Read8(ACCEL_ADDRESS, ACCEL_OUT_Y_H_A) << 8) +
       Accel_Read8(ACCEL_ADDRESS, ACCEL_OUT_Y_L_A);
  *z = (Accel_Read8(ACCEL_ADDRESS, ACCEL_OUT_Z_H_A) << 8) +
       Accel_Read8(ACCEL_ADDRESS, ACCEL_OUT_Z_L_A);
}
