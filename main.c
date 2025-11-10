#include "cyi2c.h"
#include <stdint.h>
#include <stddef.h>

#define LSM303AGR_ACC_ADDR  0x19
#define LSM303AGR_MAG_ADDR  0x1E

/* Forward declarations */
static void delay_ms(uint32_t ms);
static void lsm303agr_init(void);
static void lsm303agr_read_accel(int16_t *x, int16_t *y, int16_t *z);

int main(void)
{
    uint8_t who_am_i = 0;
    int16_t ax, ay, az;

    /* 1️⃣ Initialize I2C */
    cyi2c_init();

    delay_ms(10);

    /* 2️⃣ Read WHO_AM_I register */
    cyi2c_rd_bytes(LSM303AGR_ACC_ADDR, 0x0F, &who_am_i, 1);

    /* Check ID (should be 0x33) */
    if (who_am_i == 0x33)
    {
        // Sensor detected, initialize
        lsm303agr_init();
    }

    while (1)
    {
        lsm303agr_read_accel(&ax, &ay, &az);

        // At this point, ax/ay/az hold raw acceleration data
        // You can set a breakpoint or view in debugger
        delay_ms(100);
    }
}

/* ---- LSM303AGR Setup ---- */

static void lsm303agr_init(void)
{
    uint8_t ctrl1 = 0x57; // ODR=100Hz, all axes enable
    cyi2c_wr_bytes(LSM303AGR_ACC_ADDR, 0x20, &ctrl1, 1);
}

static void lsm303agr_read_accel(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buf[6];
    cyi2c_rd_bytes(LSM303AGR_ACC_ADDR, 0x28 | 0x80, buf, 6); // auto-increment bit

    *x = (int16_t)((buf[1] << 8) | buf[0]);
    *y = (int16_t)((buf[3] << 8) | buf[2]);
    *z = (int16_t)((buf[5] << 8) | buf[4]);
}

/* Simple delay */
static void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 40000; i++)
        __asm__("nop");
}
