/**
 * @file veml6030.c
 * @author Case Chrisbacher
 * @date April 18th, 2021
 * @brief VEML6030 Driver
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "veml6030.h"

//***********************************************************************************
// defined files
//***********************************************************************************
static uint32_t data_v;

//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  Opens I2C specifically for the VEML 6030
 *
 *
 * @details
 *	Initializes an I2C_OPEN_STRUCT with the specifications of the VEML6030. This maps out the locations for the SCL and SDA and also creates a callback
 *	for the scheduler. I2C open is then called, starting the process of opening i2c for the VEML6030
 *
 * @note
 *
 *
 ******************************************************************************/
void veml6030_i2c_open(void){
I2C_OPEN_STRUCT veml_values;
	veml_values.enable = 1;
	veml_values.master = 1;
	veml_values.refFreq = VEML_REF_FREQ;
	veml_values.freq = VEML_FREQ;
	veml_values.clhr = VEML_CLHR;
	veml_values.sclp_en = 1;
	veml_values.sdap_en = 1;
	veml_values.scl_loc = VEML_SCL_LOC;
	veml_values.sda_loc = VEML_SDA_LOC;
	veml_values.event_cb = VEML6030_READ_CB;

	i2c_open(I2Cx, &veml_values);

}


void veml6030_read(uint32_t callback){
	i2c_start(I2Cy, VEML_slave , ambient_light, &data_v, callback, 2, true, true);
}

float veml6030_rel_light(void){
	return (float)data_v;
}



