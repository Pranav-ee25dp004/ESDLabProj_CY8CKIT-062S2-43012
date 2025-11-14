#ifndef _CY_GPIO_REG_HAL_H_
#define _CY_GPIO_REG_HAL_H_

#include <stdint.h>

/* GPIO pin indices for Port 9 */
#define EPD_RST_PIN_SHIFT   (0u)   // P10_0
#define EPD_BUSY_PIN_SHIFT  (1u)   // P10_1

/* Function Prototypes */
void DEV_GPIO_Init(void);
void DEV_Digital_Write(uint8_t pin, uint8_t value);
uint8_t DEV_Digital_Read(uint8_t pin);

#endif
