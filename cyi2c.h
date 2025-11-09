#ifndef CYI2C_H
#define CYI2C_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------
// Includes
// -----------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

// -----------------------------------------------------
// Macros / Definitions
// -----------------------------------------------------
#include <stddef.h>

/* ----------------------------- Register addresses --------------------------- */
/* -------------------- Assumed / Known register addresses -------------------- */
/* You gave: PERI_CLOCK_CTL0 = 0x40000C00 */
#define PERI_CLOCK_CTL0      (*(volatile uint32_t *)0x40000C00u)

/* HSIOM select registers for P6.x (from your earlier list) */
#define HSIOM_PRT6_PORT_SEL0 (*(volatile uint32_t *)0x40300060u) /* P6.0 */
#define HSIOM_PRT6_PORT_SEL1 (*(volatile uint32_t *)0x40300064u) /* P6.1 */

/* GPIO Port 6 config registers */
#define GPIO_PRT6_CFG        (*(volatile uint32_t *)0x40310344u) /* port cfg (3 bits per pin) */
#define GPIO_PRT6_CFG_IN     (*(volatile uint32_t *)0x40310348u) /* input buffer enable */

/* Assumptions (replace according to your TRM if different): */
#define HSIOM_SELECT_SCB0_I2C  (0x12u)   /* ASSUMED: HSIOM value to select SCB0 I2C on P6.x */
#define PERI_SCB0_CLOCK_EN_BIT (1u << 0) /* ASSUMED: bit to enable SCB0 in PERI_CLOCK_CTL0 */


/* ----------------------------- Register addresses --------------------------- */
#define SCB0_CTRL            (0x40600000u)
#define SCB0_SPI_CTRL        (0x40600020u)
#define SCB0_SPI_STATUS      (0x40600024u)
#define SCB0_UART_CTRL       (0x40600040u)
#define SCB0_UART_TX_CTRL    (0x40600044u)
#define SCB0_UART_RX_CTRL    (0x40600048u)
#define SCB0_UART_RX_STATUS  (0x4060004Cu)
#define SCB0_UART_FLOW_CTRL  (0x40600050u)
#define SCB0_I2C_CTRL        (0x40600060u)
#define SCB0_I2C_STATUS      (0x40600064u)
#define SCB0_I2C_M_CMD       (0x40600068u)
#define SCB0_I2C_S_CMD       (0x4060006Cu)
#define SCB0_I2C_CFG         (0x40600070u)

#define SCB0_TX_CTRL         (0x40600200u)
#define SCB0_TX_FIFO_CTRL    (0x40600204u)
#define SCB0_TX_FIFO_STATUS  (0x40600208u)
#define SCB0_TX_FIFO_WR      (0x40600240u)

#define SCB0_RX_CTRL         (0x40600300u)
#define SCB0_RX_FIFO_CTRL    (0x40600304u)
#define SCB0_RX_FIFO_STATUS  (0x40600308u)
#define SCB0_RX_MATCH        (0x40600310u)
#define SCB0_RX_FIFO_RD      (0x40600340u)
#define SCB0_RX_FIFO_RD_SILENT (0x40600344u)

#define SCB0_EZ_DATA0        (0x40600400u) /* 256 bytes buffer */

#define SCB0_INTR_CAUSE      (0x40600E00u)

#define SCB0_INTR_M          (0x40600F00u)
#define SCB0_INTR_M_SET      (0x40600F04u)
#define SCB0_INTR_M_MASK     (0x40600F08u)
#define SCB0_INTR_M_MASKED   (0x40600F0Cu)

#define SCB0_INTR_S          (0x40600F40u)
#define SCB0_INTR_S_SET      (0x40600F44u)
#define SCB0_INTR_S_MASK     (0x40600F48u)
#define SCB0_INTR_S_MASKED   (0x40600F4Cu)

#define SCB0_INTR_TX         (0x40600F80u)
#define SCB0_INTR_TX_SET     (0x40600F84u)
#define SCB0_INTR_TX_MASK    (0x40600F88u)
#define SCB0_INTR_TX_MASKED  (0x40600F8Cu)

#define SCB0_INTR_RX         (0x40600FC0u)
#define SCB0_INTR_RX_SET     (0x40600FC4u)
#define SCB0_INTR_RX_MASK    (0x40600FC8u)
#define SCB0_INTR_RX_MASKED  (0x40600FCCu)

/* ----------------------------- Register helpers ---------------------------- */
#define REG32(addr) (*(volatile uint32_t *)(addr))

static inline uint32_t SCB0_ReadReg(uint32_t addr) {
    return REG32(addr);
}
static inline void SCB0_WriteReg(uint32_t addr, uint32_t val) {
    REG32(addr) = val;
}
static inline void SCB0_SetBits(uint32_t addr, uint32_t mask) {
    REG32(addr) |= mask;
}
static inline void SCB0_ClearBits(uint32_t addr, uint32_t mask) {
    REG32(addr) &= ~mask;
}

/* ------------------------------- Bit masks ---------------------------------- */
/* NOTE: The following masks are placeholders. Replace with TRM definitions. */

/* SCB0_I2C_CTRL bits */
#define I2C_CTRL_MASTER_MODE      (1u << 0) /* REPLACE_WITH_TRM */
#define I2C_CTRL_SLAVE_MODE       (1u << 1) /* REPLACE_WITH_TRM */
#define I2C_CTRL_ENABLED          (1u << 31) /* e.g. enable SCB block */

/* SCB0_I2C_M_CMD bits (master commands) */
#define I2C_M_CMD_START           (1u << 0) /* generate START */
#define I2C_M_CMD_STOP            (1u << 1) /* generate STOP */
#define I2C_M_CMD_ACK             (1u << 2) /* send ACK after read */
#define I2C_M_CMD_NACK            (1u << 3) /* send NACK after read */
#define I2C_M_CMD_BUSY            (1u << 31) /* placeholder */

/* SCB0_I2C_STATUS bits */
#define I2C_STATUS_M_READ         (1u << 0) /* Master read active */
#define I2C_STATUS_M_WR_DONE      (1u << 1) /* Master write finished */
#define I2C_STATUS_M_RD_DONE      (1u << 2) /* Master read finished */
#define I2C_STATUS_BUS_BUSY       (1u << 8) /* Bus busy */

/* TX/RX FIFO status bits (placeholders) */
#define TX_FIFO_NOT_FULL          (1u << 0)
#define TX_FIFO_EMPTY             (1u << 1)
#define RX_FIFO_NOT_EMPTY         (1u << 0)

/* Interrupt bits (placeholders) */
#define INTR_M_I2C_NACK           (1u << 0) /* NACK on address/data */
#define INTR_M_I2C_ARB_LOST       (1u << 1) /* Arbitration lost */
#define INTR_M_I2C_STOP           (1u << 2)
#define INTR_M_I2C_DONE           (1u << 3)

// -----------------------------------------------------
// Type Definitions (structs, enums, typedefs)
// -----------------------------------------------------
// typedef struct {
//     int x;
//     int y;
// } Point;

// -----------------------------------------------------
// Function Prototypes
// -----------------------------------------------------
/* ------------------------------ I2C functions --------------------------------
 * Note: these functions use the SCB0 TX/RX FIFOs and master command register.
 * They are simplified for clarity — for production use, adjust timeouts,
 * interrupt handling, and error propagation as required.
 */

/* Initialize SCB0 for I2C master operation. 'speed_hz' is used to pick standard or fast mode.
 * This function must also configure the pins (P6.0 SCL, P6.1 SDA) in the GPIO/HSIOM
 * configuration — that code is platform-specific so it is shown as comments/placeholders.
 */
int I2C_Init(uint32_t speed_hz);

/* Low-level: write one byte to TX FIFO and kick the master write command.
 * Returns 0 on success, negative on error.
 */
static int I2C_MasterWriteByteNow(uint8_t byte, uint32_t timeout);

/* Public API: write a single byte to a register of a slave device.
 * This issues: START + (slave_addr<<1 | 0) + reg + data + STOP
 */
int I2C_WriteByte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);

/* Public API: write multiple bytes starting at a register address.
 * Sequence: START + addr(write) + reg + data[0..len-1] + STOP
 */
int I2C_WriteMulti(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t len);

/* Read one byte from a register of a slave device */
int I2C_ReadByte(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data_out);

/* Read multiple bytes starting at a register address */
int I2C_ReadMulti(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t len);

/* -------------------------- Read helper APIS for user ------------------------ */
/* Simply return the register contents — useful for debugging/ISR handlers. */
uint32_t SCB0_Read_INTR_M(void);
uint32_t SCB0_Read_INTR_M_SET(void);
uint32_t SCB0_Read_INTR_M_MASK(void);
uint32_t SCB0_Read_INTR_M_MASKED(void);

uint32_t SCB0_Read_INTR_S(void);
uint32_t SCB0_Read_INTR_S_SET(void);
uint32_t SCB0_Read_INTR_S_MASK(void);
uint32_t SCB0_Read_INTR_S_MASKED(void);

uint32_t SCB0_Read_INTR_TX(void);
uint32_t SCB0_Read_INTR_TX_SET(void);
uint32_t SCB0_Read_INTR_TX_MASK(void);
uint32_t SCB0_Read_INTR_TX_MASKED(void);

uint32_t SCB0_Read_INTR_RX(void);
uint32_t SCB0_Read_INTR_RX_SET(void);
uint32_t SCB0_Read_INTR_RX_MASK(void);
uint32_t SCB0_Read_INTR_RX_MASKED(void);

uint32_t SCB0_Read_I2C_STATUS(void);
uint32_t SCB0_Read_I2C_M_CMD(void);


#ifdef __cplusplus
}
#endif

#endif // CYI2C_H
