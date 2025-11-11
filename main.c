/*******************************************************************************
* File Name:   main.c
* Description: I2C Master example modified to interface with LSM303AGR sensor.
*******************************************************************************/

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define I2C_FREQ            (400000UL)

/* LSM303AGR I2C Addresses */
#define LSM303AGR_ACC_ADDR  (0x19)  // Accelerometer
#define LSM303AGR_MAG_ADDR  (0x1E)  // Magnetometer

/* Accelerometer Registers */
#define LSM303AGR_WHO_AM_I_A   0x0F
#define LSM303AGR_CTRL_REG1_A  0x20
#define LSM303AGR_OUT_X_L_A    0x28

/* Magnetometer Registers */
#define LSM303AGR_WHO_AM_I_M   0x4F
#define LSM303AGR_CFG_REG_A_M  0x60
#define LSM303AGR_OUTX_L_REG_M 0x68

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void handle_error(uint32_t status);
void i2c_write_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t data);
uint8_t i2c_read_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg);
void i2c_read_bytes(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);

/*******************************************************************************
* Function Definitions
*******************************************************************************/
void handle_error(uint32_t status)
{
    if (status != CY_RSLT_SUCCESS)
        CY_ASSERT(0);
}

/* Write single register */
void i2c_write_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    handle_error(cyhal_i2c_master_write(i2c, addr, buf, 2, 0, true));
}

/* Read single register */
uint8_t i2c_read_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg)
{
    uint8_t value;
    handle_error(cyhal_i2c_master_write(i2c, addr, &reg, 1, 0, true));
    handle_error(cyhal_i2c_master_read(i2c, addr, &value, 1, 0, true));
    return value;
}

/* Read multiple bytes */
void i2c_read_bytes(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    handle_error(cyhal_i2c_master_write(i2c, addr, &reg, 1, 0, true));
    handle_error(cyhal_i2c_master_read(i2c, addr, buf, len, 0, true));
}

/*******************************************************************************
* Main
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    cyhal_i2c_t mI2C;
    cyhal_i2c_cfg_t mI2C_cfg = { .is_slave = false, .address = 0, .frequencyhal_hz = I2C_FREQ };

    result = cybsp_init();
    handle_error(result);

    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200);
    handle_error(result);

    printf("\x1b[2J\x1b[;H");
    printf("=== LSM303AGR I2C Example ===\r\n");

    /* Initialize I2C */
    handle_error(cyhal_i2c_init(&mI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL));
    handle_error(cyhal_i2c_configure(&mI2C, &mI2C_cfg));

    /* Check WHO_AM_I registers */
    uint8_t who_am_i_a = i2c_read_byte(&mI2C, LSM303AGR_ACC_ADDR, LSM303AGR_WHO_AM_I_A);
    uint8_t who_am_i_m = i2c_read_byte(&mI2C, LSM303AGR_MAG_ADDR, LSM303AGR_WHO_AM_I_M);
    printf("Accelerometer WHO_AM_I: 0x%02X (expected 0x33)\r\n", who_am_i_a);
    printf("Magnetometer WHO_AM_I:  0x%02X (expected 0x40)\r\n", who_am_i_m);

    /* Initialize Accelerometer (normal mode, 100Hz, all axes on) */
    i2c_write_byte(&mI2C, LSM303AGR_ACC_ADDR, LSM303AGR_CTRL_REG1_A, 0x57);

    /* Initialize Magnetometer (continuous conversion mode) */
    i2c_write_byte(&mI2C, LSM303AGR_MAG_ADDR, LSM303AGR_CFG_REG_A_M, 0x00);

    /* Read sensor data loop */
    while (true)
    {
        uint8_t acc_buf[6];
        uint8_t mag_buf[6];
        int16_t ax, ay, az, mx, my, mz;

        i2c_read_bytes(&mI2C, LSM303AGR_ACC_ADDR, LSM303AGR_OUT_X_L_A | 0x80, acc_buf, 6);
        i2c_read_bytes(&mI2C, LSM303AGR_MAG_ADDR, LSM303AGR_OUTX_L_REG_M, mag_buf, 6);

        ax = (int16_t)(acc_buf[1] << 8 | acc_buf[0]);
        ay = (int16_t)(acc_buf[3] << 8 | acc_buf[2]);
        az = (int16_t)(acc_buf[5] << 8 | acc_buf[4]);

        mx = (int16_t)(mag_buf[1] << 8 | mag_buf[0]);
        my = (int16_t)(mag_buf[3] << 8 | mag_buf[2]);
        mz = (int16_t)(mag_buf[5] << 8 | mag_buf[4]);

        printf("ACC [mg]: X=%d, Y=%d, Z=%d | MAG [raw]: X=%d, Y=%d, Z=%d\r\n",
              ax, ay, az, mx, my, mz);

        /* Convert raw acceleration data to m/s^2 (assuming ±2g range) */
# define ACC_SCALING_FACTOR 0.0005987f // 16 bit data and +/-2g range
        float ax_mps2 = ax * ACC_SCALING_FACTOR;
        float ay_mps2 = ay * ACC_SCALING_FACTOR;
        float az_mps2 = az * ACC_SCALING_FACTOR;

        printf("ACC [m/s2]: X=%f, Y=%f, Z=%f \r\n",
        		ax_mps2, ay_mps2, az_mps2);

        cyhal_system_delay_ms(500);
    }
}
