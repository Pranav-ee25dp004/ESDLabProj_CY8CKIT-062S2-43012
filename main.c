/*******************************************************************************
 * File Name:   main.c
 * Description: ePaper 1.9" segment display demo for Infineon PSoC6
 *******************************************************************************/

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "EPD_1in9.h"
#include "EPD_Test.h"
#include "DEV_Config.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
//#define I2C_FREQ            (400000UL)
#define I2C_FREQ (100000UL)
//#define I2C_FREQ  (50000UL) // 50 kHz
//#define I2C_FREQ (10000UL) // 10 kHz
//#define I2C_FREQ    (20000UL) // 5 kHz


/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/
void handle_error(uint32_t status);
//void i2c_write_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t data);
//uint8_t i2c_read_byte(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg);
//void i2c_read_bytes(cyhal_i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
cyhal_i2c_t mI2C;

/*******************************************************************************
 * Function Definitions
 *******************************************************************************/
void handle_error(uint32_t status)
{
	if (status != CY_RSLT_SUCCESS)
		CY_ASSERT(0);
}



/*******************************************************************************
 * Main
 *******************************************************************************/
int main(void)
{
	cy_rslt_t result;
	cyhal_i2c_cfg_t i2c_cfg = {
			.is_slave = false,
			.address = 0,
			.frequencyhal_hz = I2C_FREQ
	};

	/* === System Initialization === */
	result = cybsp_init();
	handle_error(result);

	DEV_GPIO_Init();

	//result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200);
	//handle_error(result);

	//printf("\x1b[2J\x1b[;H");
	//printf("=== 1.9 inch ePaper Display Test ===\r\n");

	/* Initialize I2C interface */
	handle_error(cyhal_i2c_init(&mI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL));
	handle_error(cyhal_i2c_configure(&mI2C, &i2c_cfg));

	/* Initialize GPIO and module power */
	if (DEV_Module_Init() != 0)
	{
		//printf("DEV_Module_Init failed!\r\n");
		CY_ASSERT(0);
	}

	epaperStartup();

	//printf("Running ePaper test sequence...\r\n");

	/* === Run ePaper test === */
	//EPD_1in9_test();
	//EPD_1in9_();

	sensor_reading myData;
	myData.humidity = 56.7;
	myData.temp = 153.9;
	myData.hum_unit = PERCENT;
	myData.temp_unit = DEGREE_CELSIUS;

	displaySensorData(&myData);

	//EPD_1in9_test();

	//printf("ePaper test complete. Entering idle loop.\r\n");

	/* === Idle Loop === */
	while (true)
	{
		cyhal_system_delay_ms(1000);
	}
}

