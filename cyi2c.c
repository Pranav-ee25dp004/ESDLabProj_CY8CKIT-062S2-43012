/* I2C Master (SCB0) — Register-level driver for CY8CKIT-062S2-43012
 *
 * Uses SCB0 registers (addresses provided by user). This file implements:
 *  - low-level register read/write helpers
 *  - I2C_Init(speed)
 *  - I2C_WriteByte(slave, reg, data)
 *  - I2C_WriteMulti(slave, reg, *data, len)
 *  - generic SCB0 register read functions (for interrupts/status)
 *
 * IMPORTANT: This code uses the register addresses you provided.
 * Bit masks and field encodings are placeholders. Replace macros marked
 * with "REPLACE_WITH_TRM" with actual bit positions/values from the
 * PSoC6 TRM / device header.
 */

#include <stdint.h>
#include <stddef.h>
#include "cyi2c.h"


/* ---------------------------------------------------------
   Interrupt helper utilities
   --------------------------------------------------------- */
/* --------------------------- Utility / timing ------------------------------- */

/* Simple timeout loop — adjust if you have a timer. Returns 0 on timeout, 1 on OK */
static int wait_for_condition(volatile uint32_t *reg, uint32_t mask, int expect_set, uint32_t timeout)
{
    while (timeout--) {
        uint32_t v = *reg;
        if (expect_set) {
            if (v & mask) return 1;
        } else {
            if ((v & mask) == 0) return 1;
        }
    }
    return 0;
}
/* Wait for a specific interrupt bit to be set in INTR_M_MASKED register */
static int wait_for_intr_masked(uint32_t mask, uint32_t timeout)
{
    while (timeout--)
    {
        if (SCB0_ReadReg(SCB0_INTR_M_MASKED) & mask)
            return 0; /* success */
    }
    return -1; /* timeout */
}

/* Check if a NACK occurred and clear interrupt sources */
static int check_and_clear_nack(void)
{
    uint32_t intr = SCB0_ReadReg(SCB0_INTR_M_MASKED);

    if (intr & INTR_M_I2C_NACK)
    {
        /* Clear NACK interrupt */
        SCB0_WriteReg(SCB0_INTR_M, INTR_M_I2C_NACK);
        return -1; /* signal NACK failure */
    }

    /* Clear everything we used */
    SCB0_WriteReg(SCB0_INTR_M, intr);
    return 0;
}


/* ------------------------------ I2C functions --------------------------------
 * Note: these functions use the SCB0 TX/RX FIFOs and master command register.
 * They are simplified for clarity — for production use, adjust timeouts,
 * interrupt handling, and error propagation as required.
 */

/* Initialize SCB0 for I2C master operation. 'speed_hz' is used to pick standard or fast mode.
 * This function must also configure the pins (P6.0 SCL, P6.1 SDA) in the GPIO/HSIOM
 * configuration — that code is platform-specific so it is shown as comments/placeholders.
 */


/* I2C_Init: sets up P6.0/P6.1 for SCB0 I2C, enables clock, configures SCB0 registers */
int I2C_Init(uint32_t speed_hz)
{
    /* ------------------ 0) Sanity: avoid accidental invalid memory access ------------- */
    /* If these addresses are invalid for your silicon, update them now per TRM. */
    /* ------------ 1) Enable SCB0 peripheral clock (PERI) --------------------------- */
    PERI_CLOCK_CTL0 |= PERI_SCB0_CLOCK_EN_BIT;
    /* NOTE: If your TRM requires a specific sequence (e.g. write key + divider), use that. */

    /* ------------ 2) Configure HSIOM for P6.0 (SCL) and P6.1 (SDA) ------------------ */
    /* Route pins to SCB0 I2C function. Replace HSIOM_SELECT_SCB0_I2C with TRM value if different. */
    HSIOM_PRT6_PORT_SEL0 = HSIOM_SELECT_SCB0_I2C; /* P6.0 -> SCB0_I2C_SCL */
    HSIOM_PRT6_PORT_SEL1 = HSIOM_SELECT_SCB0_I2C; /* P6.1 -> SCB0_I2C_SDA */

    /* ------------ 3) Configure drive mode for P6.0/P6.1 to Open-Drain (Drive low) - */
    /* GPIO_PRT6_CFG: 3 bits per pin DM field: bitpos = pin * 3.
     * We set DM = 6 (0b110) which is "Open-Drain, drives low" on PSoC6 series.
     * Clear then set only P6.0 & P6.1 DM bits.
     */
    uint32_t cfg = GPIO_PRT6_CFG;
    cfg &= ~((0x7u << (0 * 3)) | (0x7u << (1 * 3)));   /* clear P6.0 & P6.1 DM */
    cfg |=  ((0x6u << (0 * 3)) | (0x6u << (1 * 3)));   /* set DM = 6 (OD-LO) for P6.0 & P6.1 */
    GPIO_PRT6_CFG = cfg;

    /* Enable input buffer for both pins so peripheral can sample SDA/SCL */
    GPIO_PRT6_CFG_IN |= (1u << 0) | (1u << 1);

    /* ------------------ 4) Disable SCB block while configuring ---------------------- */
    SCB0_ClearBits(SCB0_CTRL, I2C_CTRL_ENABLED);

    /* ------------------ 5) Set SCB0 to I2C master mode ---------------------------- */
    SCB0_WriteReg(SCB0_I2C_CTRL, I2C_CTRL_MASTER_MODE);

    /* ------------------ 6) Configure I2C timing (placeholder fields) --------------- */
    /* Replace SCB0_I2C_CFG values with TRM/pdl-calculated timing parameters */
    if (speed_hz <= 100000u) {
        /* Standard mode 100 kHz (placeholder) */
        SCB0_WriteReg(SCB0_I2C_CFG, 0x00000010u); /* REPLACE_WITH_TRM */
    } else {
        /* Fast mode 400 kHz (placeholder) */
        SCB0_WriteReg(SCB0_I2C_CFG, 0x00000020u); /* REPLACE_WITH_TRM */
    }

    /* ------------------ 7) FIFO and Interrupt init -------------------------------- */
    SCB0_WriteReg(SCB0_TX_FIFO_CTRL, 0u);
    SCB0_WriteReg(SCB0_RX_FIFO_CTRL, 0u);

    /* Clear any pending interrupts (write-to-clear for these regs) */
    SCB0_WriteReg(SCB0_INTR_M, 0xFFFFFFFFu);
    SCB0_WriteReg(SCB0_INTR_S, 0xFFFFFFFFu);
    SCB0_WriteReg(SCB0_INTR_TX, 0xFFFFFFFFu);
    SCB0_WriteReg(SCB0_INTR_RX, 0xFFFFFFFFu);

    /* Optionally enable the commonly used master interrupt masks (replace masks per TRM) */
    /* SCB0_WriteReg(SCB0_INTR_M_MASK, INTR_M_I2C_NACK | INTR_M_I2C_DONE | INTR_M_I2C_STOP | INTR_M_I2C_ARB_LOST); */

    /* ------------------ 8) Enable SCB block -------------------------------------- */
    SCB0_SetBits(SCB0_CTRL, I2C_CTRL_ENABLED);

    return 0; /* success */
}

/* Low-level: write one byte to TX FIFO and kick the master write command.
 * Returns 0 on success, negative on error.
 */
static int I2C_MasterWriteByteNow(uint8_t byte, uint32_t timeout)
{
    /* Wait until TX FIFO not full */
    if (!wait_for_condition((volatile uint32_t *)SCB0_TX_FIFO_STATUS, TX_FIFO_NOT_FULL, 1, timeout))
        return -1; /* timeout */

    /* Push byte into TX FIFO */
    SCB0_WriteReg(SCB0_TX_FIFO_WR, (uint32_t)byte);

    /* In many SCB implementations writing to TX FIFO is enough; the hardware
     * will perform the transfer once addressed. If a master command is required
     * to tell the controller to issue a WRITE transaction, set it here.
     * For example: SCB0_SetBits(SCB0_I2C_M_CMD, I2C_M_CMD_WRITE);
     */

    return 0;
}

/* Public API: write a single byte to a register of a slave device.
 * This issues: START + (slave_addr<<1 | 0) + reg + data + STOP
 */
int I2C_WriteByte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    uint32_t timeout = 1000000u;
    uint32_t cmd;

    /* 1) Ensure bus idle */
    if (!wait_for_condition((volatile uint32_t *)SCB0_I2C_STATUS, I2C_STATUS_BUS_BUSY, 0, timeout))
        return -1; /* bus busy timeout */

    /* 2) Push slave address (write) into TX FIFO
     * The I2C address byte is (slave_addr << 1) | 0 (write)
     */
    if (I2C_MasterWriteByteNow((uint8_t)(slave_addr << 1), 100000) != 0) return -2;

    /* 3) Generate START (master command) if required by HW */
    cmd = I2C_M_CMD_START; /* Placeholder */
    SCB0_WriteReg(SCB0_I2C_M_CMD, cmd);

    /* 4) Wait for ACK/NACK via master interrupt/status
     * Check INTR_M or I2C_STATUS for NACK/ACK — placeholders below.
     */
    /* Wait for TX to drain or for specific master event */
    if (!wait_for_condition((volatile uint32_t *)SCB0_INTR_M_MASKED, INTR_M_I2C_DONE, 1, 1000000u)) {
        return -3; /* timeout */
    }

    /* Check for NACK */
    if (SCB0_ReadReg(SCB0_INTR_M_MASKED) & INTR_M_I2C_NACK) {
        /* Clear NACK */
        SCB0_WriteReg(SCB0_INTR_M, INTR_M_I2C_NACK);
        return -4; /* NACK received */
    }

    /* 5) Send register address */
    if (I2C_MasterWriteByteNow(reg_addr, 100000) != 0) return -5;
    if (!wait_for_condition((volatile uint32_t *)SCB0_INTR_M_MASKED, INTR_M_I2C_DONE, 1, 1000000u)) return -6;
    if (SCB0_ReadReg(SCB0_INTR_M_MASKED) & INTR_M_I2C_NACK) {
        SCB0_WriteReg(SCB0_INTR_M, INTR_M_I2C_NACK);
        return -7;
    }

    /* 6) Send data byte */
    if (I2C_MasterWriteByteNow(data, 100000) != 0) return -8;
    if (!wait_for_condition((volatile uint32_t *)SCB0_INTR_M_MASKED, INTR_M_I2C_DONE, 1, 1000000u)) return -9;
    if (SCB0_ReadReg(SCB0_INTR_M_MASKED) & INTR_M_I2C_NACK) {
        SCB0_WriteReg(SCB0_INTR_M, INTR_M_I2C_NACK);
        return -10;
    }

    /* 7) Generate STOP */
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_STOP);

    return 0; /* success */
}

/* Public API: write multiple bytes starting at a register address.
 * Sequence: START + addr(write) + reg + data[0..len-1] + STOP
 */
int I2C_WriteMulti(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t len)
{
    uint32_t timeout = 1000000u;
    size_t i;

    /* 1) Ensure bus idle */
    if (!wait_for_condition((volatile uint32_t *)SCB0_I2C_STATUS, I2C_STATUS_BUS_BUSY, 0, timeout))
        return -1; /* bus busy timeout */

    /* 2) Send address (write) */
    if (I2C_MasterWriteByteNow((uint8_t)(slave_addr << 1), 100000) != 0) return -2;
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_START);
    if (!wait_for_condition((volatile uint32_t *)SCB0_INTR_M_MASKED, INTR_M_I2C_DONE, 1, timeout)) return -3;
    if (SCB0_ReadReg(SCB0_INTR_M_MASKED) & INTR_M_I2C_NACK) { SCB0_WriteReg(SCB0_INTR_M, INTR_M_I2C_NACK); return -4; }

    /* 3) Send register address */
    if (I2C_MasterWriteByteNow(reg_addr, 100000) != 0) return -5;
    if (!wait_for_condition((volatile uint32_t *)SCB0_INTR_M_MASKED, INTR_M_I2C_DONE, 1, timeout)) return -6;
    if (SCB0_ReadReg(SCB0_INTR_M_MASKED) & INTR_M_I2C_NACK) { SCB0_WriteReg(SCB0_INTR_M, INTR_M_I2C_NACK); return -7; }

    /* 4) Send data bytes */
    for (i = 0; i < len; ++i) {
        if (I2C_MasterWriteByteNow(data[i], 100000) != 0) return -8 - (int)i;
        if (!wait_for_condition((volatile uint32_t *)SCB0_INTR_M_MASKED, INTR_M_I2C_DONE, 1, timeout)) return -9 - (int)i;
        if (SCB0_ReadReg(SCB0_INTR_M_MASKED) & INTR_M_I2C_NACK) { SCB0_WriteReg(SCB0_INTR_M, INTR_M_I2C_NACK); return -10 - (int)i; }
    }

    /* 5) STOP */
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_STOP);

    return 0;
}

/* Read one byte from a register of a slave device */
int I2C_ReadByte(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data_out)
{
    const uint32_t bus_timeout = 1000000u;
    const uint32_t byte_timeout = 100000u;

    if (!data_out) return -100;

    /* Clear interrupts */
    SCB0_WriteReg(SCB0_INTR_M, 0xFFFFFFFFu);

    /* Ensure bus idle */
    if (!wait_for_condition((volatile uint32_t *)SCB0_I2C_STATUS, I2C_STATUS_BUS_BUSY, 0, bus_timeout))
        return -1;

    /* Send slave address + WRITE and START */
    I2C_MasterWriteByteNow((uint8_t)(slave_addr << 1), byte_timeout);
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_START);
    if (!wait_for_intr_masked(INTR_M_I2C_DONE, bus_timeout)) return -2;
    if (check_and_clear_nack()) return -3;

    /* Send register address */
    I2C_MasterWriteByteNow(reg_addr, byte_timeout);
    if (!wait_for_intr_masked(INTR_M_I2C_DONE, bus_timeout)) return -4;
    if (check_and_clear_nack()) return -5;

    /* REPEATED START + slave address (READ) */
    I2C_MasterWriteByteNow((uint8_t)((slave_addr << 1) | 1u), byte_timeout);
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_START); /* repeated START */
    if (!wait_for_intr_masked(INTR_M_I2C_DONE, bus_timeout)) return -6;
    if (check_and_clear_nack()) return -7;

    /* Wait for RX FIFO has data */
    if (!wait_for_condition((volatile uint32_t *)SCB0_RX_FIFO_STATUS, RX_FIFO_NOT_EMPTY, 1, byte_timeout))
        return -8;

    *data_out = (uint8_t)SCB0_ReadReg(SCB0_RX_FIFO_RD);

    /* Send NACK for final byte + STOP */
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_NACK | I2C_M_CMD_STOP);

    return 0;
}

/* Read multiple bytes starting at a register address */
int I2C_ReadMulti(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t len)
{
    const uint32_t bus_timeout = 1000000u;
    const uint32_t byte_timeout = 100000u;
    size_t i;

    if (!data || len == 0) return -100;

    /* Clear interrupts */
    SCB0_WriteReg(SCB0_INTR_M, 0xFFFFFFFFu);

    /* Ensure bus idle */
    if (!wait_for_condition((volatile uint32_t *)SCB0_I2C_STATUS, I2C_STATUS_BUS_BUSY, 0, bus_timeout))
        return -1;

    /* Address + WRITE + START */
    I2C_MasterWriteByteNow((uint8_t)(slave_addr << 1), byte_timeout);
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_START);
    if (!wait_for_intr_masked(INTR_M_I2C_DONE, bus_timeout)) return -2;
    if (check_and_clear_nack()) return -3;

    /* Send register address */
    I2C_MasterWriteByteNow(reg_addr, byte_timeout);
    if (!wait_for_intr_masked(INTR_M_I2C_DONE, bus_timeout)) return -4;
    if (check_and_clear_nack()) return -5;

    /* REPEATED START + READ address */
    I2C_MasterWriteByteNow((uint8_t)((slave_addr << 1) | 1u), byte_timeout);
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_START);
    if (!wait_for_intr_masked(INTR_M_I2C_DONE, bus_timeout)) return -6;
    if (check_and_clear_nack()) return -7;

    for (i = 0; i < len; ++i) {

        /* Wait until RX FIFO has data */
        if (!wait_for_condition((volatile uint32_t *)SCB0_RX_FIFO_STATUS, RX_FIFO_NOT_EMPTY, 1, byte_timeout))
            return -8 - (int)i;

        data[i] = (uint8_t)SCB0_ReadReg(SCB0_RX_FIFO_RD);

        /* ACK all but last byte */
        if (i < (len - 1)) {
            SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_ACK);
        }
    }

    /* Final NACK + STOP */
    SCB0_WriteReg(SCB0_I2C_M_CMD, I2C_M_CMD_NACK | I2C_M_CMD_STOP);

    return 0;
}

/* -------------------------- Read helper APIS for user ------------------------ */
/* Simply return the register contents — useful for debugging/ISR handlers. */
uint32_t SCB0_Read_INTR_M(void)        { return SCB0_ReadReg(SCB0_INTR_M); }
uint32_t SCB0_Read_INTR_M_SET(void)    { return SCB0_ReadReg(SCB0_INTR_M_SET); }
uint32_t SCB0_Read_INTR_M_MASK(void)   { return SCB0_ReadReg(SCB0_INTR_M_MASK); }
uint32_t SCB0_Read_INTR_M_MASKED(void) { return SCB0_ReadReg(SCB0_INTR_M_MASKED); }

uint32_t SCB0_Read_INTR_S(void)        { return SCB0_ReadReg(SCB0_INTR_S); }
uint32_t SCB0_Read_INTR_S_SET(void)    { return SCB0_ReadReg(SCB0_INTR_S_SET); }
uint32_t SCB0_Read_INTR_S_MASK(void)   { return SCB0_ReadReg(SCB0_INTR_S_MASK); }
uint32_t SCB0_Read_INTR_S_MASKED(void) { return SCB0_ReadReg(SCB0_INTR_S_MASKED); }

uint32_t SCB0_Read_INTR_TX(void)        { return SCB0_ReadReg(SCB0_INTR_TX); }
uint32_t SCB0_Read_INTR_TX_SET(void)    { return SCB0_ReadReg(SCB0_INTR_TX_SET); }
uint32_t SCB0_Read_INTR_TX_MASK(void)   { return SCB0_ReadReg(SCB0_INTR_TX_MASK); }
uint32_t SCB0_Read_INTR_TX_MASKED(void) { return SCB0_ReadReg(SCB0_INTR_TX_MASKED); }

uint32_t SCB0_Read_INTR_RX(void)        { return SCB0_ReadReg(SCB0_INTR_RX); }
uint32_t SCB0_Read_INTR_RX_SET(void)    { return SCB0_ReadReg(SCB0_INTR_RX_SET); }
uint32_t SCB0_Read_INTR_RX_MASK(void)   { return SCB0_ReadReg(SCB0_INTR_RX_MASK); }
uint32_t SCB0_Read_INTR_RX_MASKED(void) { return SCB0_ReadReg(SCB0_INTR_RX_MASKED); }

uint32_t SCB0_Read_I2C_STATUS(void)    { return SCB0_ReadReg(SCB0_I2C_STATUS); }
uint32_t SCB0_Read_I2C_M_CMD(void)      { return SCB0_ReadReg(SCB0_I2C_M_CMD); }

/* ------------------------------ Notes / To-dos ------------------------------ */
/* - Replace all "REPLACE_WITH_TRM" and placeholder masks with actual
 *   values from the PSoC6 TRM / device header file.
 * - Configure HSIOM for P6.0/P6.1 to connect pins to SCB0 I2C peripheral.
 * - The current code polls INTR_M_MASKED for completion. For higher
 *   performance/robustness, consider using interrupts (enable/mask INTR_M bits
 *   and service them in an ISR) or using the SCB status registers.
 * - If SCB requires an explicit "WRITE_CMD" to transfer TX FIFO content to the
 *   bus, add that write (some SCB variants do this implicitly).
 * - Add error codes and more precise timeout handling as needed.
 */

/* End of file */
