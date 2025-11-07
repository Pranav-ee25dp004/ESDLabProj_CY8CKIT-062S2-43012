#include <stdint.h>

/* Register Addresses */
#define HSIOM_PRT0_PORT_SEL1   (*(volatile unsigned long *)(0x40300004))

#define GPIO_PRT0_OUT          (*(volatile unsigned long *)(0x40310000))
#define GPIO_PRT0_OUT_CLR      (*(volatile unsigned long *)(0x40310004))
#define GPIO_PRT0_OUT_SET      (*(volatile unsigned long *)(0x40310008))
#define GPIO_PRT0_OUT_INV      (*(volatile unsigned long *)(0x4031000C))
#define GPIO_PRT0_CFG          (*(volatile unsigned long *)(0x40310044))

#define GPIO_PRT0_IN (*(volatile unsigned long *) (0x40310010))

/* GREEN LED on P0[5] — Active Low */
#define LED_PIN         5u
#define LED_MASK        (1u << LED_PIN)

/* HSIOM: IO5_SEL bits [12:8] in PORT_SEL1 */
#define LED_HSIOM_SHIFT (8u)
#define LED_HSIOM_MASK  (0x1Fu << LED_HSIOM_SHIFT)

/* HSIOM: IO5_SEL bits [4:0] in PORT_SEL1 */
#define SW_HSIOM_MASK  (0x01Fu)

/* Drive Mode for P0[5]: DRIVE_MODE5 bits [22:20] */
#define LED_CFG_SHIFT   (20u)
#define LED_CFG_MASK    (0x7u << LED_CFG_SHIFT)

/* IN_EN4 and Drive Mode for P0[4]: IN_EN4[19] DRIVE_MODE5 bits [18:16] */
#define SW_CFG_SHIFT   (16u)
#define SW_CFG_MASK    (0xFu << SW_CFG_SHIFT)

#define SW_PIN_MASK   (1u << 4)


int main(void)
{
    /* 1) Route P0[5] and P0[4] to GPIO */
    HSIOM_PRT0_PORT_SEL1 &= ~(LED_HSIOM_MASK | SW_HSIOM_MASK);

    /* 2) Configure P0[5] drive mode = STRONG */
    GPIO_PRT0_CFG &= ~(LED_CFG_MASK | SW_CFG_MASK);
    GPIO_PRT0_CFG |=  (0x6u << LED_CFG_SHIFT);

    /* 3) Configure P0[4] IN_EN4 = 1, drive mode = HIGHZ */
    GPIO_PRT0_CFG |=  (0x8u << SW_CFG_SHIFT);

    /* 4) Turn LED OFF (because LED is active-low) */
    GPIO_PRT0_OUT_SET = LED_MASK;

    /* Switch input mask (P0.4) */
    uint8_t prevSwState = 0;

    while(1)
    {
    	uint8_t swState = (GPIO_PRT0_IN & SW_PIN_MASK);

    	if((prevSwState != 0) && (swState == 0)){
        /* 4) Toggle LED */
        GPIO_PRT0_OUT_INV = LED_MASK;
    	}

    	prevSwState = swState;

        /* Delay */
        for(volatile unsigned long i = 0; i < 3000; i++);
    }
}
