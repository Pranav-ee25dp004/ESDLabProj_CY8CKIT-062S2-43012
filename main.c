#include <stdint.h>
#include <stdio.h>
#include "cyi2c.h"

/* LSM303AGR I2C slave addresses */
#define LSM303_ACC_ADDR   0x19
#define LSM303_MAG_ADDR   0x1E

/* Accelerometer registers */
#define CTRL_REG1_A       0x20
#define CTRL_REG4_A       0x23
#define OUT_X_L_A         0x28  /* needs auto-increment */

/* Magnetometer registers */
#define CFG_REG_A_M       0x60
#define CFG_REG_C_M       0x62
#define OUTX_L_REG_M      0x68  /* needs auto-increment */

/* Init accelerometer */
static int LSM303_Init_Accel(void)
{
    int ret;
    ret = I2C_WriteByte(LSM303_ACC_ADDR, CTRL_REG1_A, 0x57); /* 100Hz, normal, XYZ enable */
    if (ret) return ret;

    ret = I2C_WriteByte(LSM303_ACC_ADDR, CTRL_REG4_A, 0x00); /* ±2G full scale */
    return ret;
}

/* Init magnetometer */
static int LSM303_Init_Mag(void)
{
    int ret;
    ret = I2C_WriteByte(LSM303_MAG_ADDR, CFG_REG_A_M, 0x00); /* Continuous mode */
    if (ret) return ret;

    ret = I2C_WriteByte(LSM303_MAG_ADDR, CFG_REG_C_M, 0x00); /* Defaults OK */
    return ret;
}

/* Read accelerometer XYZ (mg units) */
static int LSM303_Read_Accel(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t raw[6];
    int ret = I2C_ReadMulti(LSM303_ACC_ADDR, OUT_X_L_A | 0x80, raw, 6); /* auto-increment */
    if (ret) return ret;

    *x = (int16_t)((raw[1] << 8) | raw[0]);
    *y = (int16_t)((raw[3] << 8) | raw[2]);
    *z = (int16_t)((raw[5] << 8) | raw[4]);
    return 0;
}

/* Read magnetometer XYZ */
static int LSM303_Read_Mag(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t raw[6];
    int ret = I2C_ReadMulti(LSM303_MAG_ADDR, OUTX_L_REG_M | 0x80, raw, 6);
    if (ret) return ret;

    *x = (int16_t)((raw[1] << 8) | raw[0]);
    *y = (int16_t)((raw[3] << 8) | raw[2]);
    *z = (int16_t)((raw[5] << 8) | raw[4]);
    return 0;
}

/* ---------------- MAIN ---------------- */
int main(void)
{
    int ret;
    int16_t ax, ay, az;
    int16_t mx, my, mz;

    /* Init SCB0 I2C */
    I2C_Init(100000); /* 100kHz */

    /* LSM303AGR init */
    ret = LSM303_Init_Accel();
    if (ret) while(1); /* error: hang or debug print */

    ret = LSM303_Init_Mag();
    if (ret) while(1);

    while (1)
    {
        LSM303_Read_Accel(&ax, &ay, &az);
        LSM303_Read_Mag(&mx, &my, &mz);

        /* Replace with UART / SWO / printf according to your environment */
        printf("ACC: %d %d %d   MAG: %d %d %d\n", ax, ay, az, mx, my, mz);

        /* Small delay */
        for (volatile int i = 0; i < 100000; i++);
    }
}
