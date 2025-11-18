/*****************************************************************************
* | File      	:	  EPD_Test.h
* | Author      :   Waveshare team
* | Function    :   e-Paper test Demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-11
* | Info        :   
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
#ifndef _EPD_TEST_H_
#define _EPD_TEST_H_

#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"
#include <stdlib.h> // malloc() free()


typedef enum {
    DEGREE_CELSIUS,
    DEGREE_FAHRENHEIT
} tmp_unit;

typedef enum {
    PERCENT,
    G_PER_CUBIC_METER
} hm_unit;


typedef struct _sensor_reading{
	float temp;
	float humidity;
	tmp_unit temp_unit;
	hm_unit hum_unit;
}sensor_reading;

int EPD_1in9_test(void);
void epaperStartup();
void displaySensorData(sensor_reading* data);
int extract_hum_digits(float n, int d[]);
int extract_temp_digits(float n, int d[]);

#endif
