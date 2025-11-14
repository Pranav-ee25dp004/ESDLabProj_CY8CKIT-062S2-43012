/*******************************************************************************
* File Name:   DEV_Config.c
* Description: Hardware interface configuration for Infineon (I2C + GPIO)
*              Adapted from STM HAL version to use ModusToolbox I2C APIs
*              and user-defined I2C helper functions from main.c
*******************************************************************************/

#include "DEV_Config.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <stdio.h>
#include "cy_gpio_reg_hal.h"
#include "cyhal_i2c.h"


/*******************************************************************************
* External references
*******************************************************************************/
extern cyhal_i2c_t mI2C;  // I2C handle defined and initialized in main.c
extern void i2c_write_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t data);
extern void i2c_write_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t data);
extern uint8_t i2c_read_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg);
//extern void i2c_read_bytes(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);


/*******************************************************************************
* Function Name: DEV_I2C_WriteByte
********************************************************************************
* Summary:
*  Writes one byte over I2C to the given address.
*
* Parameters:
*  ADD   - 7-bit I2C device address
*  value - Byte to be written
*******************************************************************************/
void DEV_I2C_WriteByte(UBYTE ADD, UBYTE value)
{
	cyhal_i2c_master_write(&mI2C, ADD, &value, 1, 100, true);
}

/*******************************************************************************
* Function Name: DEV_I2C_ReadByte
********************************************************************************
* Summary:
*  Reads one byte over I2C from the given address.
*
* Parameters:
*  ADD - 7-bit I2C device address
*
* Return:
*  The byte read from device
*******************************************************************************/
UBYTE DEV_I2C_ReadByte(UBYTE ADD)
{
    uint8_t value = 0;
    cyhal_i2c_master_read(&mI2C, ADD, &value, 1, 100, true);
    return value;
}


/*******************************************************************************
* Function Name: DEV_Module_Init
********************************************************************************
* Summary:
*  Initializes GPIO pins and prepares hardware for display operation.
*******************************************************************************/
int DEV_Module_Init(void)
{
    // Configure any required GPIOs here (if not already done in BSP)
    DEV_Digital_Write(EPD_RST_PIN_SHIFT, 1);

    //printf("DEV_Module_Init: Hardware interface initialized.\r\n");
    return 0;
}

/*******************************************************************************
* Function Name: DEV_Module_Exit
********************************************************************************
* Summary:
*  Deinitializes or powers down the ePaper hardware.
*******************************************************************************/
void DEV_Module_Exit(void)
{
    // Power off / reset hardware
    DEV_Digital_Write(EPD_RST_PIN_SHIFT, 0);
    //printf("DEV_Module_Exit: Hardware interface powered down.\r\n");
}
