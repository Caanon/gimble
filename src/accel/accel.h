// Functions for the LSM303DLHC 3-Axis Accelerometer and Magnetometer.
// Datasheet source: http://www.adafruit.com/datasheets/LSM303DLHC.PDF
#ifndef __ACCEL_ACCEL_H__
#define __ACCEL_ACCEL_H__

#define ACCEL_ADDRESS 0x19
#define MAG_ADDRESS 0x1E

// Accelerometer registers
#define ACCEL_CTRL_REG1_A 0x20
#define ACCEL_CTRL_REG2_A 0x21
#define ACCEL_CTRL_REG3_A 0x22
#define ACCEL_CTRL_REG4_A 0x23
#define ACCEL_CTRL_REG5_A 0x24
#define ACCEL_CTRL_REG6_A 0x25
#define ACCEL_REFERENCE_A 0x26
#define ACCEL_STATUS_REG_A 0x27
#define ACCEL_OUT_X_L_A 0x28
#define ACCEL_OUT_X_H_A 0x29
#define ACCEL_OUT_Y_L_A 0x2A
#define ACCEL_OUT_Y_H_A 0x2B
#define ACCEL_OUT_Z_L_A 0x2C
#define ACCEL_OUT_Z_H_A 0x2D
#define ACCEL_FIFO_CTRL_REG_A 0x2E
#define ACCEL_FIFO_SRC_REG_A 0x2F
#define ACCEL_INT1_CFG_A 0x30
#define ACCEL_INT1_SOURCE_A 0x31
#define ACCEL_INT1_THS_A 0x32
#define ACCEL_INT1_DURATION_A 0x33
#define ACCEL_INT2_CFG_A 0x34
#define ACCEL_INT2_SOURCE_A 0x35
#define ACCEL_INT2_THS_A 0x36
#define ACCEL_INT2_DURATION_A 0x37
#define ACCEL_CLICK_CFG_A 0x38
#define ACCEL_CLICK_SRC_A 0x39
#define ACCEL_CLICK_THS_A 0x3A
#define ACCEL_TIME_LIMIT_A 0x3B

// Initialize the accelerometer. Assumes the I2C bus has been set up correctly.
void Accel_Init();

// Read the raw values from the accelerometer's sensors; no conversion is
// performed. Note: does NOT control for saturation yet.
void Accel_ReadRaw(int *x, int *y, int *z);

#endif // __ACCEL_ACCEL_H__
