/**
 * @file main.c
 * @brief Example application using the cyi2c API to communicate with LSM303AGR.
 */

#include "cyi2c.h"
#include <stdint.h>

#define LSM303AGR_ADDR      0x19u
#define LSM303AGR_WHO_AM_I  0x0Fu
#define LSM303AGR_CTRL1     0x20u
#define LSM303AGR_OUT_X_L_A 0x28u

int main(void)
{
    uint8_t who_am_i = 0;
    uint8_t accel_buf[6] = {0};

    /* Initialize I2C hardware */
    cyi2c_init();

    /* Read device ID */
    cyi2c_rd_bytes(LSM303AGR_ADDR, LSM303AGR_WHO_AM_I, &who_am_i, 1);

    /* Configure accelerometer (enable XYZ axes) */
    cyi2c_wr_byte(LSM303AGR_ADDR, LSM303AGR_CTRL1, 0x57u);

    /* Read accelerometer data */
    cyi2c_rd_bytes(LSM303AGR_ADDR, (LSM303AGR_OUT_X_L_A | 0x80u), accel_buf, 6);

    while (1)
    {
        /* Add your main loop code here */
        cy_delay(1000000u);
    }

    return 0;
}
