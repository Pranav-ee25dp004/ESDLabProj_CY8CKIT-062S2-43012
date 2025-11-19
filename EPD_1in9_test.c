/*****************************************************************************
 * | File      	:   EPD_2in13b_V3_test.c
 * | Author      :   Waveshare team
 * | Function    :   2.13inch B V3 e-paper test demo
 * | Info        :
 *----------------
 * |	This version:   V1.0
 * | Date        :   2020-04-13
 * | Info        :
 * -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
 ******************************************************************************/
#include "EPD_Test.h"
#include "EPD_1in9.h"

const char digtCode[20][2] = {
		{ 0xbf	, 0x1f },	//Digit 	0
		{ 0x00	, 0x1f },	//Digit 	1
		{ 0xfd	, 0x17 },	//Digit 	2
		{ 0xf5	, 0x1f },	//Digit 	3
		{ 0x47	, 0x1f },	//Digit 	4
		{ 0xf7	, 0x1d },	//Digit 	5
		{ 0xff	, 0x1d },	//Digit 	6
		{ 0x21	, 0x1f },	//Digit 	7
		{ 0xff	, 0x1f },	//Digit 	8
		{ 0xf7	, 0x1f },	//Digit 	9

		{ 0x7f , 0x1f }, //Char A
		{ 0xFF , 0x0A }, //Char B
		{ 0xBF , 0x11 }, //Char C
		{ 0xBF , 0x0E }, //Char D
		{ 0xFF , 0x11 }, //Char E
		{ 0x7F , 0x01 }, //Char F
		{ 0xBF , 0x1C }, //Char G
		{ 0x5F , 0x1f }  //Char H

};


char i2cData[15] = {0x00,  // Temp Digit 3
		0x00, 0x00, // Temp Digit 2
		0x00, 0x00, // Temp Digit 1
		0x00, 0x00, // Temp Digit 0
		0x00, 0x00, // Hum Digit 2
		0x00, 0x00, // Hum Digit 1
		0x00, 0x00, // Hum Digit
		0x00, 0x00,};  // 1

sensor_reading senseData ={0};


unsigned char b[15];

void a(const unsigned char* image_buffer)
{
	for(uint8_t i = 0 ; i < 15 ; i++)
	{
		b[i] = image_buffer[i];
	}
}


int EPD_1in9_test(void)
{
	//printf("EPD_1IN9_test Demo\r\n");
	if(DEV_Module_Init()!=0){
		return -1;
	}

	//printf("e-Paper Init and Clear...\r\n");
	EPD_1in9_init();

	EPD_1in9_lut_5S();
	a(DSPNUM_1in9_off);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	EPD_1in9_lut_GC();

	a(DSPNUM_1in9_on);
	EPD_1in9_Write_Screen1(b);
	DEV_Delay_ms(500);

	a(DSPNUM_1in9_off);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	EPD_1in9_lut_DU_WB();
	a(DSPNUM_1in9_W0);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W1);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W2);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W3);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W4);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W5);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W6);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W7);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W8);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);
	a(DSPNUM_1in9_W9);
	EPD_1in9_Write_Screen(b);

	// Added by Pranav
	a(DSPNUM_1in9_WAc); // A
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	a(DSPNUM_1in9_WBc); // B
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	a(DSPNUM_1in9_WCc); // E
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	a(DSPNUM_1in9_WDc); // D
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);


	a(DSPNUM_1in9_WEc); //E
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	a(DSPNUM_1in9_WFc); // F
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	a(DSPNUM_1in9_WGc); // G
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	a(DSPNUM_1in9_WHc); // H
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	/*	for(b[1]=0xFF; b[1]>=0; b[1] = b[1]>>1){

		EPD_1in9_Write_Screen(b);
		DEV_Delay_ms(500);

	}*/


	a(DSPNUM_1in9_WB);
	EPD_1in9_Write_Screen(b);
	DEV_Delay_ms(500);

	// EPD_1in9_SendCommand(0xE1);
	// EPD_1in9_SendCommand(0xE5);
	// EPD_1in9_SendCommand(0x73);

	// char g= 0 ;
	// g = EPD_1in9_readCommand(0xE4);
	// //printf("%d\r\n",g);


	EPD_1in9_sleep();
	return 0;

}

void epaperStartup()
{
	// Load special LUT for deep cleaning
	EPD_1in9_lut_5S();

	// Load a blank screen image (all segments off)
	a(DSPNUM_1in9_off);

	// Perform a full clear (slow but removes ghosting)
	EPD_1in9_Write_Screen(b);

	DEV_Delay_ms(500);   // allow ink particles to settle

	// Load normal LUT for regular operation
	EPD_1in9_lut_GC();

	EPD_1in9_lut_DU_WB();
}

void displaySensorData(sensor_reading* data){

	int digits[4] = {0};

	// Temperature Digits

	//if(fabs(data->temp-senseData.temp) > 0.05){
	senseData.temp = data->temp;
	extract_temp_digits(senseData.temp, digits);

	if(digits[3] == 0){
		i2cData[0] = 0x00;

	}
	else{

		i2cData[0] = digtCode[digits[3]][1];
	}


	i2cData[1] = digtCode[digits[2]][0];
	i2cData[2] = digtCode[digits[2]][1];
	i2cData[3] = digtCode[digits[1]][0];
	i2cData[4] = digtCode[digits[1]][1] | ((digits[0] % 2 == 0) ? 0b00100000 : 0x00); /* decimal point */;
	i2cData[11] = digtCode[digits[0]][0];
	i2cData[12] = digtCode[digits[0]][1];
	//}

	// Humidity Digits

	//if(fabs(data->humidity-senseData.humidity) > 0.05){
	senseData.humidity = data->humidity;
	extract_hum_digits(senseData.humidity, digits);
	// humidity digits
	i2cData[5] = digtCode[digits[2]][0];
	i2cData[6] = digtCode[digits[2]][1];
	i2cData[7] = digtCode[digits[1]][0];
	i2cData[8] = digtCode[digits[1]][1] | ((digits[0] % 2 != 0) ? 0b00100000 : 0x00); /* decimal point */;
	i2cData[9] = digtCode[digits[0]][0];
	i2cData[10] = digtCode[digits[0]][1] | ((digits[0] % 3 == 0) ? 0b00100000 : 0x00); /* percentage sign */;

	//}

	// special symbols - °C / °F, bluetooth, battery
	//i2cData[13] = 0x05 /* °C */ | 0b00001000 /* bluetooth */ | 0b00010000 /* battery icon */;
	//i2cData[13] = 0x06 /* °F */ | 0b00001000 /* bluetooth */ | 0b00010000 /* battery icon */;

	i2cData[13] =
		    /* degree symbol */
		    ((digits[0] == 5 || digits[0] == 6 || digits[0] == 7) ? digits[0] : 0x00)

		    /* bluetooth (bit 3) */
		    | ((digits[0] % 4 == 0) ? 0b00001000 : 0x00)

		    /* battery (bit 4) */
		    | ((digits[0] == 1) ? 0b00010000 : 0x00);



	//a(i2cData);
	EPD_1in9_Write_Screen(i2cData);
	DEV_Delay_ms(500);
}


int extract_temp_digits(float n, int d[]) {
	if (n < 0){
		d[0] = 0;               // fractional
		d[1] = 0;         // ones
		d[2] = 0;   // tens
		d[3] = 0;        // hundreds (0–1)

		return -1;
	}

	if (n > 199.9){
		d[0] = 9;               // fractional
		d[1] = 9;         // ones
		d[2] = 9;   // tens
		d[3] =  1;        // hundreds (0–1)

		return -1;
	}

	int i = (int)n;              // integer part
	int f = (int)((n - i) * 10); // fractional digit

	d[0] = f;               // fractional
	d[1] =  i % 10;         // ones
	d[2] = (i / 10) % 10;   // tens
	d[3] =  i / 100;        // hundreds (0–1)

	return 4; // always 4 digits filled
}

int extract_hum_digits(float n, int d[]) {
	if (n < 0){
		d[0] = 0;               // fractional
		d[1] = 0;         // ones
		d[2] = 0;   // tens
		d[3] = 0;        // hundreds (0–1)
		return -1;
	}

	if (n > 99.9){
		d[0] = 9;               // fractional
		d[1] = 9;         // ones
		d[2] = 9;   // tens
		d[3] = 0;        // hundreds (0–1)
		return -2;
	}

	int i = (int)n;              // integer part
	int f = (int)((n - i) * 10); // fractional digit

	d[0] = f;               // fractional
	d[1] =  i % 10;         // ones
	d[2] = (i / 10) % 10;   // tens
	d[3] =  0;        // hundreds (0–1)

	return 4; // always 4 digits filled
}









