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

//#define PERI_GR6_CLOCK_CTL  (*(volatile uint32_t *)0x400040C0)
#define PERI_DIV_CMD (*(volatile uint32_t *)0x40000400) //Divider command
#define PERI_CLOCK_CTL3 (*(volatile uint32_t *)0x40000C0C) //Clock control
#define PERI_DIV_16_CTL3 (*(volatile uint32_t *)0x4000140C) //Divider control (for 16.0 divider). See PERI_DIV_16_CTL0 for the details of bit fields.

#define SCB3_CTRL (*(volatile uint32_t *)0x40630000) //Generic control. See SCB0_CTRL for the details of bit fields.
#define SCB3_I2C_CTRL (*(volatile uint32_t *)0x40630060)       //I2C control. See #define SCB0_I2C_CTRL for the details of bit fields.
#define SCB3_I2C_STATUS (*(volatile uint32_t *)0x40630064)     //I2C status. See #define SCB0_I2C_STATUS for the details of bit fields.
#define SCB3_I2C_M_CMD (*(volatile uint32_t *)0x40630068)      //I2C master command. See #define SCB0_I2C_M_CMD for the details of bit fields.
#define SCB3_I2C_S_CMD (*(volatile uint32_t *)0x4063006C)     //I2C slave command. See #define SCB0_I2C_S_CMD for the details of bit fields.
#define SCB3_I2C_CFG (*(volatile uint32_t *)0x40630070)      //I2C configuration. See #define SCB0_I2C_CFG for the details of bit fields.

#define SCB3_TX_CTRL (*(volatile uint32_t *)0x40630200) //Transmitter control. See SCB0_TX_CTRL for the details of bit fields.
#define SCB3_TX_FIFO_CTRL (*(volatile uint32_t *)0x40630204) //Transmitter FIFO control. See SCB0_TX_FIFO_CTRL for the details of bit fields.
#define SCB3_TX_FIFO_STATUS (*(volatile uint32_t *)0x40630208) //Transmitter FIFO status. See SCB0_TX_FIFO_STATUS for the details of bit fields.
#define SCB3_TX_FIFO_WR (*(volatile uint32_t *)0x40630240) //Transmitter FIFO write. See SCB0_TX_FIFO_WR for the details of bit fields.
#define SCB3_RX_CTRL (*(volatile uint32_t *)0x40630300) //Receiver control. See SCB0_RX_CTRL for the details of bit fields.
#define SCB3_RX_FIFO_CTRL (*(volatile uint32_t *)0x40630304) //Receiver FIFO control. See SCB0_RX_FIFO_CTRL for the details of bit fields.
#define SCB3_RX_FIFO_STATUS (*(volatile uint32_t *)0x40630308) //Receiver FIFO status. See SCB0_RX_FIFO_STATUS for the details of bit fields.
#define SCB3_RX_MATCH (*(volatile uint32_t *)0x40630310) //Slave address and mask. See SCB0_RX_MATCH for the details of bit fields.
#define SCB3_RX_FIFO_RD (*(volatile uint32_t *)0x40630340) //Receiver FIFO read. See SCB0_RX_FIFO_RD for the details of bit fields.
#define SCB3_RX_FIFO_RD_SILENT (*(volatile uint32_t *)0x40630344) //Receiver FIFO read silent. See SCB0_RX_FIFO_RD_SILENT for the details of bit fields.


#define SCB3_I2C_M_CMD (*(volatile uint32_t *)(0x40630068)) // I2C master command. See SCB0_I2C_M_CMD for the details of bit fields.

/* GPIO CFG Regs */
#define  HSIOM_PRT6_PORT_SEL0 (*(volatile uint32_t *)0x40300060) //Port selection 0
#define GPIO_PRT6_CFG (*(volatile uint32_t*)0x40310344) //Port configuration register. See GPIO_PRT2_CFG for the details of bit fields.

/* ========== Bit Masks ========== */
#define I2C_STATUS_BUS_BUSY_MASK   (1u << 0)
#define I2C_STATUS_M_READ_MASK     (1u << 5)


/* PERI Bit positions */
#define PERI_DIV_CMD_ENABLE         (1u << 31)
#define PERI_DIV_CMD_DISABLE        (1u << 30)
#define PERI_DIV_CMD_TYPE_SEL_SHIFT 8
#define PERI_DIV_CMD_DIV_SEL_SHIFT  0
#define PERI_DIV_CMD_PA_TYPE_SHIFT  24
#define PERI_DIV_CMD_PA_DIV_SHIFT   16
#define PERI_CLOCK_CTL_ENABLE       (1u << 31)

/* ================================
 * Public API Prototypes
 * ================================ */

/**
 * @brief Initialize the I2C hardware (divider, pins, SCB mode setup).
 */
void cyi2c_init(void);

/**
 * @brief Write multiple bytes to a device register.
 * @param slave_addr 7-bit I2C slave address.
 * @param reg_addr   Starting register address.
 * @param data       Pointer to data buffer.
 * @param len        Number of bytes to write.
 * @return 0 on success, <0 on error.
 */
int cyi2c_wr_bytes(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t len);


int cyi2c_rd_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buf, size_t len);

/**
 * @brief Small delay utility (busy wait).
 * @param cycles Number of cycles to delay.
 */
void cy_delay(uint32_t cycles);

#endif /* CYI2C_H */
