/*******************************************************************************
* File Name:   DEV_GPIO_HAL.c
* Description: Bare-metal GPIO HAL for ePaper using Port 10
*              (P10_0 = RST, P10_1 = BUSY)
*******************************************************************************/

#include <stdint.h>
#include "cy_gpio_reg_hal.h"
#include "cyhal.h"
#include "cybsp.h"
#include "DEV_Config.h"

/* =========================================================
 * Port 10 Register Map (Real Addresses)
 * ========================================================= */
#define HSIOM_PRT10_PORT_SEL0   (*(volatile uint32_t *)(0x403000A0))

#define GPIO_PRT10_OUT          (*(volatile uint32_t *)(0x40310500))
#define GPIO_PRT10_OUT_CLR      (*(volatile uint32_t *)(0x40310504))
#define GPIO_PRT10_OUT_SET      (*(volatile uint32_t *)(0x40310508))
#define GPIO_PRT10_OUT_INV      (*(volatile uint32_t *)(0x4031050C))
#define GPIO_PRT10_IN           (*(volatile uint32_t *)(0x40310510))
#define GPIO_PRT10_INTR         (*(volatile uint32_t *)(0x40310514))
#define GPIO_PRT10_INTR_MASK    (*(volatile uint32_t *)(0x40310518))
#define GPIO_PRT10_INTR_MASKED  (*(volatile uint32_t *)(0x4031051C))
#define GPIO_PRT10_INTR_SET     (*(volatile uint32_t *)(0x40310520))
#define GPIO_PRT10_INTR_CFG     (*(volatile uint32_t *)(0x40310540))
#define GPIO_PRT10_CFG          (*(volatile uint32_t *)(0x40310544))
#define GPIO_PRT10_CFG_IN       (*(volatile uint32_t *)(0x40310548))
#define GPIO_PRT10_CFG_OUT      (*(volatile uint32_t *)(0x4031054C))

/* =========================================================
 * Pin Configuration (P10_0 = RST, P10_1 = BUSY)
 * ========================================================= */
#define EPD_RST_PIN_SHIFT      (0u)
#define EPD_BUSY_PIN_SHIFT     (1u)

#define EPD_RST_PIN_MASK       (1u << EPD_RST_PIN_SHIFT)
#define EPD_BUSY_PIN_MASK      (1u << EPD_BUSY_PIN_SHIFT)

/* HSIOM: IO0_SEL bits [4:0], IO1_SEL bits [12:8] */
#define EPD_RST_HSIOM_MASK     (0x1Fu)         /* bits [4:0] for P10_0 */
#define EPD_BUSY_HSIOM_MASK    (0x1Fu << 8)    /* bits [12:8] for P10_1 */

/* Drive-mode bits in CFG register */
#define EPD_RST_CFG_SHIFT      (0u)            /* DRIVE_MODE0 bits [2:0] */
#define EPD_BUSY_CFG_SHIFT     (4u)            /* DRIVE_MODE1 bits [6:4] */
#define EPD_RST_CFG_MASK       (0x7u << EPD_RST_CFG_SHIFT)
#define EPD_BUSY_CFG_MASK      (0xFu << EPD_BUSY_CFG_SHIFT)

/*******************************************************************************
* Function Name: DEV_GPIO_Init
********************************************************************************
* Summary:
*   Initializes GPIO pins for RST (output, P10_0) and BUSY (input, P10_1).
*******************************************************************************/
void DEV_GPIO_Init(void)
{
    /* 1. Route P10_0 (RST) and P10_1 (BUSY) to GPIO function */
    HSIOM_PRT10_PORT_SEL0 &= ~(EPD_RST_HSIOM_MASK | EPD_BUSY_HSIOM_MASK);

    /* 2. Configure drive modes:
          - P10_0: strong drive output
          - P10_1: high-Z input with input enable */
    GPIO_PRT10_CFG &= ~(EPD_RST_CFG_MASK | EPD_BUSY_CFG_MASK);
    GPIO_PRT10_CFG |=  (0x6u << EPD_RST_CFG_SHIFT);  /* Strong drive for RST */
    GPIO_PRT10_CFG |=  (0x8u << EPD_BUSY_CFG_SHIFT); /* Input enable + High-Z for BUSY */

    /* 3. Ensure RST pin starts high (inactive) */
    /*while(1){
    	GPIO_PRT10_OUT_SET = EPD_RST_PIN_MASK;
    	DEV_Delay_ms(500);
    	GPIO_PRT10_OUT_SET = EPD_BUSY_PIN_MASK;
    	DEV_Delay_ms(500);
    	GPIO_PRT10_OUT_CLR = EPD_RST_PIN_MASK;
    	DEV_Delay_ms(500);
    	GPIO_PRT10_OUT_CLR = EPD_BUSY_PIN_MASK;
    	DEV_Delay_ms(500);
    }*/
}

/*******************************************************************************
* Function Name: DEV_Digital_Write
********************************************************************************
* Summary:
*   Sets or clears the RST output pin.
*******************************************************************************/
void DEV_Digital_Write(uint8_t pin, uint8_t value)
{
    if (pin == EPD_RST_PIN_SHIFT)
    {
        if (value)
            GPIO_PRT10_OUT_SET = EPD_RST_PIN_MASK;
        else
            GPIO_PRT10_OUT_CLR = EPD_RST_PIN_MASK;
    }
}

/*******************************************************************************
* Function Name: DEV_Digital_Read
********************************************************************************
* Summary:
*   Reads logic level from BUSY input pin.
*******************************************************************************/
uint8_t DEV_Digital_Read(uint8_t pin)
{
    if (pin == EPD_BUSY_PIN_SHIFT)
        return (GPIO_PRT10_IN & EPD_BUSY_PIN_MASK) ? 1 : 0;
    return 0;
}
