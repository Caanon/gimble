// Functions for the L3GD20H 3-Axis Gyro.
// Datasheet source: http://www.adafruit.com/datasheets/L3GD20H.pdf
#ifndef __GYRO_GYRO_H__
#define __GYRO_GYRO_H__

#define GYRO_ADDRESS 0x6B

// Registers for the
#define GYRO_WHO_AM_I 0x0F
#define GYRO_CTRL1 0x20
#define GYRO_CTRL2 0x21
#define GYRO_CTRL3 0x22
#define GYRO_CTRL4 0x23
#define GYRO_CTRL5 0x24
#define GYRO_REFERENCE 0x25
#define GYRO_OUT_TEMP 0x26
#define GYRO_STATUS 0x27
#define GYRO_OUT_X_L 0x28
#define GYRO_OUT_X_H 0x29
#define GYRO_OUT_Y_L 0x2A
#define GYRO_OUT_Y_H 0x2B
#define GYRO_OUT_Z_L 0x2C
#define GYRO_OUT_Z_H 0x2D
#define GYRO_FIFO_CTRL 0x2E
#define GYRO_FIFO_SRC 0x2F
#define GYRO_IG_CFG 0x30
#define GYRO_IG_SRC 0x31
#define GYRO_IG_THIS_XH 0x32
#define GYRO_IG_THIS_XL 0x33
#define GYRO_IG_THIS_YH 0x34
#define GYRO_IG_THIS_YL 0x35
#define GYRO_IG_THIS_ZH 0x36
#define GYRO_IG_THIS_ZL 0x37
#define GYRO_IG_DURATION 0x38
#define GYRO_LOW_ODR 0x39

#define GYRO_250_DPS 0
#define GYRO_500_DPS 1
#define GYRO_2000_DPS 2

// Initialize the gyro. Assumes the I2C bus has been set up correctly.
void Gyro_Init();

// Dump the registers to the serial port. Assumes InitUart has been called.
void Gyro_DumpRegisters();

// Read the raw values from the gyro's sensors; no conversion is performed.
// Note: does NOT control for saturation yet.
void Gyro_ReadRaw(int *x, int *y, int *z);

// Read degrees from the gyro's sensors.
// Note: does NOT control for saturation yet.
void Gyro_ReadDegrees(float *x, float *y, float *z);

// Low-level functions to read and write gyro registers.
unsigned char Gyro_ReadRegister(const unsigned char register_address);
void Gyro_WriteRegister(const unsigned char register_address,
                        const unsigned char value);

#endif // __GYRO_GYRO_H__
