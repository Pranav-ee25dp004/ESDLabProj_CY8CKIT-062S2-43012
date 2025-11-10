/**
 * @file cyi2c.c
 * @brief Implementation skeleton for PSoC6 register-level I2C driver.
 *
 * Add your own register-level code inside each API.
 */

#include "cyi2c.h"

void cy_delay(uint32_t cycles)
{
    while (cycles--) __asm__("nop");
}


static void HSIOM_cfg_PRT6(){

	uint32_t reg = HSIOM_PRT6_PORT_SEL0;

	/* Clear old fields for IO0 (bits[4:0]) and IO1 (bits[12:8]) */
	reg &= ~((0x1Fu << 0) | (0x1Fu << 8));

	/* Set both to 0xE = DS_2 = SCB3 I2C_SCL/SDA */
	reg |= (0xEu << 0);   // P6[0] = SCB3_I2C_SCL
	reg |= (0xEu << 8);   // P6[1] = SCB3_I2C_SDA

	HSIOM_PRT6_PORT_SEL0 = reg;
}

static void GPIO_cfg_PRT6(){

    uint32_t reg = GPIO_PRT6_CFG;

    /* Clear existing drive-mode + input-enable bits for pins 0 & 1 */
    reg &= ~0xFFu;            // bits [7:0]

    /* Set DM=100, IN_EN=1 for both pins 0 & 1 → 0xCC */
    reg |= 0xCCu;

    GPIO_PRT6_CFG = reg;

}

static void SCB3_cfg(){



}

void cyi2c_init(void)
{
    /* TODO:
     * 1. Setup PERI clock divider
     * 2. Enable PERI group clock for target SCB
     * 3. Configure HSIOM for I2C pins
     * 4. Configure SCB registers for I2C master/slave
     */
	// Configure SCB registers for I2C master

	HSIOM_cfg_PRT6();
	GPIO_cfg_PRT6();
	SCB3_cfg();

}

int cyi2c_wr_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    /* TODO: Implement single-byte write using I2C start → addr → reg → data → stop sequence */
    (void)slave_addr;
    (void)reg_addr;
    (void)data;
    return 0;
}

int cyi2c_wr_bytes(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t len)
{
    /* TODO: Implement multi-byte write sequence */
    (void)slave_addr;
    (void)reg_addr;
    (void)data;
    (void)len;
    return 0;
}

int cyi2c_rd_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buf, size_t len)
{
    /* TODO: Implement multi-byte read sequence (start → write reg → repeated start → read) */
    (void)slave_addr;
    (void)reg_addr;
    (void)buf;
    (void)len;
    return 0;
}
