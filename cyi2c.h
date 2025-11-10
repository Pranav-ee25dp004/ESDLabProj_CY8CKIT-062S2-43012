/**
 * @file cyi2c.h
 * @brief Simple register-level I2C driver interface for PSoC6.
 *
 * This header defines only the function prototypes and constants
 * for I2C initialization and data access. Register macros will be
 * added by the user.
 */

#ifndef CYI2C_H
#define CYI2C_H

#include <stdint.h>
#include <stddef.h>

#define PERI_GR6_CLOCK_CTL  (*(volatile uint32_t *)0x400040C0)
#define PERI_GR6_CLOCK_CTL  (*(volatile uint32_t *)0x400040C0)

#define SCB3_I2C_CTRL (*(volatile uint32_t *)0x40630060)       //I2C control. See #define SCB0_I2C_CTRL for the details of bit fields.
#define SCB3_I2C_STATUS (*(volatile uint32_t *)0x40630064)     //I2C status. See #define SCB0_I2C_STATUS for the details of bit fields.
#define SCB3_I2C_M_CMD (*(volatile uint32_t *)0x40630068)      //I2C master command. See #define SCB0_I2C_M_CMD for the details of bit fields.
#define SCB3_I2C_S_CMD (*(volatile uint32_t *)0x4063006C)     //I2C slave command. See #define SCB0_I2C_S_CMD for the details of bit fields.
#define SCB3_I2C_CFG (*(volatile uint32_t *)0x40630070)      //I2C configuration. See #define SCB0_I2C_CFG for the details of bit fields.

/* GPIO CFG Regs */
#define  HSIOM_PRT6_PORT_SEL0 (*(volatile uint32_t *)0x40300060) //Port selection 0
#define GPIO_PRT6_CFG (*(volatile uint32_t*)0x40310344) //Port configuration register. See GPIO_PRT2_CFG for the details of bit fields.

/* ================================
 * Public API Prototypes
 * ================================ */

/**
 * @brief Initialize the I2C hardware (divider, pins, SCB mode setup).
 */
void cyi2c_init(void);

/**
 * @brief Write a single byte to a device register.
 * @param slave_addr 7-bit I2C slave address.
 * @param reg_addr   Register address inside the slave.
 * @param data       Byte to write.
 * @return 0 on success, <0 on error.
 */
int cyi2c_wr_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);

/**
 * @brief Write multiple bytes to a device register.
 * @param slave_addr 7-bit I2C slave address.
 * @param reg_addr   Starting register address.
 * @param data       Pointer to data buffer.
 * @param len        Number of bytes to write.
 * @return 0 on success, <0 on error.
 */
int cyi2c_wr_bytes(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t len);

/**
 * @brief Read multiple bytes from a device register.
 * @param slave_addr 7-bit I2C slave address.
 * @param reg_addr   Starting register address.
 * @param buf        Pointer to buffer for data.
 * @param len        Number of bytes to read.
 * @return 0 on success, <0 on error.
 */
int cyi2c_rd_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buf, size_t len);

/**
 * @brief Small delay utility (busy wait).
 * @param cycles Number of cycles to delay.
 */
void cy_delay(uint32_t cycles);

#endif /* CYI2C_H */
