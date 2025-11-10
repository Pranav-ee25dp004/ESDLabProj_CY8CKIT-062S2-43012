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



/* Debug Vars */
// =========================
// Debug variables
// =========================

volatile uint32_t dbg_PERI_DIV_CMD;
volatile uint32_t dbg_PERI_DIV_16_CTL3;
volatile uint32_t dbg_PERI_CLOCK_CTL3;

volatile uint32_t dbg_HSIOM_PRT6_PORT_SEL0;
volatile uint32_t dbg_GPIO_PRT6_CFG;

volatile uint32_t dbg_SCB3_CTRL;
volatile uint32_t dbg_SCB3_I2C_CTRL;
volatile uint32_t dbg_SCB3_I2C_CFG;
volatile uint32_t dbg_SCB3_TX_CTRL;
volatile uint32_t dbg_SCB3_RX_CTRL;
volatile uint32_t dbg_SCB3_I2C_STATUS;
volatile uint32_t dbg_SCB3_I2C_M_CMD;

volatile uint32_t dbg_BUS_BUSY;
volatile uint32_t dbg_LOOP_COUNTER;

// =========================
// Debug capture function
// =========================
static void capture_debug_state(void)
{
    dbg_PERI_DIV_CMD       = PERI_DIV_CMD;
    dbg_PERI_DIV_16_CTL3   = PERI_DIV_16_CTL3;
    dbg_PERI_CLOCK_CTL3    = PERI_CLOCK_CTL3;

    dbg_HSIOM_PRT6_PORT_SEL0 = HSIOM_PRT6_PORT_SEL0;
    dbg_GPIO_PRT6_CFG        = GPIO_PRT6_CFG;

    dbg_SCB3_CTRL        = SCB3_CTRL;
    dbg_SCB3_I2C_CTRL    = SCB3_I2C_CTRL;
    dbg_SCB3_I2C_CFG     = SCB3_I2C_CFG;
    dbg_SCB3_TX_CTRL     = SCB3_TX_CTRL;
    dbg_SCB3_RX_CTRL     = SCB3_RX_CTRL;
    dbg_SCB3_I2C_STATUS  = SCB3_I2C_STATUS;
    dbg_SCB3_I2C_M_CMD   = SCB3_I2C_M_CMD;

    dbg_BUS_BUSY = (dbg_SCB3_I2C_STATUS & 1u);
    dbg_LOOP_COUNTER = 0;   // you can increment this in loops
}



static void PERI_cfg_SCB3(void)
{
    /* 1️ Configure integer divider #3 → divide by (9+1)=10 */
    PERI_DIV_16_CTL3 = (9u << 8);  // INT16_DIV = 9, EN=0 (HW will set it later)

    /* 2️ Enable divider using PERI_DIV_CMD
     * TYPE_SEL = 1 → 16-bit divider
     * DIV_SEL  = 3 → divider #3
     * PA_TYPE_SEL = 3, PA_DIV_SEL = 255 → align to clk_peri
     */
    PERI_DIV_CMD =
        (PERI_DIV_CMD_ENABLE) |
        (1u << PERI_DIV_CMD_TYPE_SEL_SHIFT) |
        (3u << PERI_DIV_CMD_DIV_SEL_SHIFT) |
        (3u << PERI_DIV_CMD_PA_TYPE_SHIFT) |
        (255u << PERI_DIV_CMD_PA_DIV_SHIFT);

    /* 3️ Connect divider #3 to SCB3 and enable clock gate */
    PERI_CLOCK_CTL3 =
        (3u & 0xFFu) | PERI_CLOCK_CTL_ENABLE;
}



static void HSIOM_cfg_PRT6(){

	uint32_t reg = HSIOM_PRT6_PORT_SEL0;

	/* Clear IO0_SEL and IO1_SEL fields (5 bits each) */
	reg &= ~((0x1Fu << 0) | (0x1Fu << 8));

	/* Set both to 0x13 (ACT_7 → SCB3 I2C) */
	reg |=  (0x13u << 0);   // P6.0 = SCB3_I2C_SCL
	reg |=  (0x13u << 8);   // P6.1 = SCB3_I2C_SDA

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

	volatile uint32_t status;
	uint32_t timeout;

	/* ===== 1️ Disable SCB before configuration ===== */
	SCB3_CTRL = 0x00000000UL;

	/* ===== 2️ Configure I2C Control (Master Mode, Oversampling) ===== */
	SCB3_I2C_CTRL =
			(1u << 31)           /* MASTER_MODE enable */
			| (1u << 9)            /* M_NOT_READY_DATA_NACK = 1 */
			| (1u << 8)            /* M_READY_DATA_ACK = 1 */
			| (8u << 4)            /* LOW_PHASE_OVS = 8 */
			| (8u << 0);           /* HIGH_PHASE_OVS = 8 */

	/* ===== 3️ Configure I2C Filter Characteristics ===== */
	SCB3_I2C_CFG =
			(1u << 12)           /* SCL_IN_FILT_SEL = 1 (50ns filter) */
			| (1u << 4)            /* SDA_IN_FILT_SEL = 1 (50ns filter) */
			| (3u << 0)            /* SDA_IN_FILT_TRIM = 3 */
			| (2u << 16)           /* SDA_OUT_FILT0_TRIM = 2 */
			| (2u << 18)           /* SDA_OUT_FILT1_TRIM = 2 */
			| (2u << 20);          /* SDA_OUT_FILT2_TRIM = 2 */

	/* ===== 4️ Configure TX/RX Data Path ===== */
	SCB3_TX_CTRL =
			(1u << 16)           /* OPEN_DRAIN = 1 (I2C standard) */
			| (1u << 8)            /* MSB_FIRST = 1 */
			| (7u << 0);           /* DATA_WIDTH = 7 → 8 bits */

	SCB3_RX_CTRL =
			(1u << 9)            /* MEDIAN filter enable */
			| (1u << 8)            /* MSB_FIRST = 1 */
			| (7u << 0);           /* DATA_WIDTH = 7 → 8 bits */

	/* ===== 5️ Clear TX and RX FIFOs ===== */
	SCB3_TX_FIFO_CTRL = (1u << 16);
	SCB3_TX_FIFO_CTRL = 0x00;
	SCB3_RX_FIFO_CTRL = (1u << 16);
	SCB3_RX_FIFO_CTRL = 0x00;

	/* ===== 6️ Enable SCB3 in I2C mode ===== */
	SCB3_CTRL =
			(1u << 31)           /* ENABLED = 1 */
			| (0u << 24)           /* MODE = I2C */
			| (1u << 11);          /* BYTE_MODE = 1 (8-bit FIFO width) */

	/* ===== 7️ Verify SCB is alive and bus is idle ===== */
	timeout = 1000000u;
	do {
		status = SCB3_I2C_STATUS;
	} while ((status & I2C_STATUS_BUS_BUSY_MASK) && --timeout);

	if (timeout == 0) {
		// ⚠ Bus stuck busy or clock not configured yet.
		// Suggest reinitializing or resetting SCB.
		// SCB3_CTRL = 0;
		// printf("I2C bus busy or stuck\n");
	} else {
		//  SCB active and bus idle
	}

	/* ===== 8️ Polling Mode Default (No Interrupts) ===== */
	// Example polling:
	// while (SCB3_I2C_STATUS & I2C_STATUS_BUS_BUSY_MASK); // wait for idle

	/* ===== 9️ (Optional) Interrupt Mode Setup ===== */
	/*
	    // Enable TX FIFO interrupt when empty
	    SCB3_INTR_TX_MASK = (1u << 4);   // TX_TRIGGER
	    // Enable RX FIFO interrupt when not empty
	    SCB3_INTR_RX_MASK = (1u << 2);   // RX_TRIGGER
	    // Enable Master interrupts (STOP, START, etc.)
	    SCB3_INTR_M_MASK = 0xFFFFFFFF;
	    // Enable NVIC interrupt if using CMSIS
	    NVIC_EnableIRQ(scb3_interrupt_IRQn);
	 */
}

static void SCB3_test_start_stop(void)
{

    /* 1️ Wait for bus to be idle before starting */
    while (SCB3_I2C_STATUS & I2C_STATUS_BUS_BUSY_MASK);

    /* 2️ Issue START when bus is idle */
    SCB3_I2C_M_CMD = (1u << 1);  // M_START_ON_IDLE
    cy_delay(1000);

    capture_debug_state();  // snapshot initial config
    /* 3️ Wait until BUS_BUSY goes high (START active) */
    while (!(SCB3_I2C_STATUS & I2C_STATUS_BUS_BUSY_MASK));

    /* 4️ Issue STOP to release the bus */
    SCB3_I2C_M_CMD = (1u << 4);  // M_STOP
    cy_delay(1000);

    /* 5️ Wait until BUS_BUSY goes low again */
    while (SCB3_I2C_STATUS & I2C_STATUS_BUS_BUSY_MASK);
}


void cyi2c_init(void)
{
    /* === 1️ Enable PERI clock for SCB3 === */
    PERI_cfg_SCB3();

    /* === 2️ Route I2C pins to SCB3 === */
    HSIOM_cfg_PRT6();

    /* === 3️ Configure GPIO drive modes for open-drain I2C === */
    GPIO_cfg_PRT6();

    /* === 4️ Configure SCB3 as I2C master === */
    SCB3_cfg();

    /* === 5️ Optional: Test START/STOP to verify hardware toggling === */
    SCB3_test_start_stop();
}


int cyi2c_wr_bytes(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t len)
{
    uint32_t timeout;

    // 1️ Wait until I2C bus is idle
    timeout = 100000;
    while ((SCB3_I2C_STATUS & 1u) && --timeout);  // BUS_BUSY=1 → active
    if (timeout == 0) return -1;  // Bus stuck busy

    // 2️ Generate START condition
    SCB3_I2C_M_CMD = (1u << 0);  // M_START

    // 3️ Write slave address + write bit (LSB=0)
    while (SCB3_TX_FIFO_STATUS & (1u << 8));  // Wait if FIFO full
    SCB3_TX_FIFO_WR = (slave_addr << 1) | 0u; // Write direction

    // 4️ Write target register address
    while (SCB3_TX_FIFO_STATUS & (1u << 8));
    SCB3_TX_FIFO_WR = reg_addr;

    // 5️ Write all payload bytes
    for (size_t i = 0; i < len; i++)
    {
        while (SCB3_TX_FIFO_STATUS & (1u << 8)); // Wait if FIFO full
        SCB3_TX_FIFO_WR = data[i];
    }

    // 6️ Wait until TX FIFO is empty (all data shifted out)
    timeout = 100000;
    while (!(SCB3_TX_FIFO_STATUS & (1u << 9)) && --timeout); // Wait EMPTY=1
    if (timeout == 0) return -2;  // TX timeout

    // 7️ Generate STOP
    SCB3_I2C_M_CMD = (1u << 4);  // M_STOP

    // 8️ Wait until bus is idle again
    timeout = 100000;
    while ((SCB3_I2C_STATUS & 1u) && --timeout);
    if (timeout == 0) return -3;  // STOP timeout

    return 0; // Success
}

int cyi2c_rd_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buf, size_t len)
{
    uint32_t timeout;

    if (len == 0)
        return 0;

    /* 1️ Wait until bus idle */
    timeout = 100000;
    while ((SCB3_I2C_STATUS & 1u) && --timeout);
    if (timeout == 0) return -1;  // bus stuck

    /* 2️ START + write slave addr */
    SCB3_I2C_M_CMD = (1u << 0);   // M_START
    while (SCB3_TX_FIFO_STATUS & (1u << 8));
    SCB3_TX_FIFO_WR = (slave_addr << 1) | 0u;  // write mode

    /* 3️ Write target register address */
    while (SCB3_TX_FIFO_STATUS & (1u << 8));
    SCB3_TX_FIFO_WR = reg_addr;

    /* 4️ Wait until FIFO empty before repeated start */
    timeout = 100000;
    while (!(SCB3_TX_FIFO_STATUS & (1u << 9)) && --timeout);
    if (timeout == 0) return -2;

    /* 5️ Repeated START + read address */
    SCB3_I2C_M_CMD = (1u << 0);   // M_START (becomes RESTART)
    while (SCB3_TX_FIFO_STATUS & (1u << 8));
    SCB3_TX_FIFO_WR = (slave_addr << 1) | 1u;  // read mode

    /* 6️ Read bytes from RX FIFO */
    for (size_t i = 0; i < len; i++)
    {
        /* Wait until at least one byte received */
        timeout = 100000;
        while (((SCB3_RX_FIFO_STATUS >> 9) & 1u) && --timeout);
        if (timeout == 0) return -3;

        /* Read from FIFO */
        buf[i] = SCB3_RX_FIFO_RD & 0xFFu;

        /* Send ACK after each byte except last */
        if (i < (len - 1))
            SCB3_I2C_M_CMD = (1u << 2);  // M_ACK
        else
            SCB3_I2C_M_CMD = (1u << 3);  // M_NACK
    }

    /* 7️ STOP condition */
    SCB3_I2C_M_CMD = (1u << 4);  // M_STOP

    /* 8️ Wait for bus idle */
    timeout = 100000;
    while ((SCB3_I2C_STATUS & 1u) && --timeout);
    if (timeout == 0) return -4;

    return 0;
}


