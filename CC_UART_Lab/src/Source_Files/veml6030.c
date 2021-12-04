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
 *  Opens I2C specifically for the
 *
 *
 * @details
 *	Initializes an I2C_OPEN_STRUCT with the specifications of the si7021. This maps out the locations for the SCL and SDA and also creates a callback
 *	for the scheduler. I2C open is then called, starting the process of opening i2c for the si7021
 *
 * @note
 *
 *
 ******************************************************************************/
void veml6030_i2c_open(void){
I2C_OPEN_STRUCT veml_values;
	veml_values.enable = 1;	//this
	veml_values.master = 1;	//this
	veml_values.refFreq = SI_REF_FREQ;	//this
	veml_values.freq = SI_FREQ;	//this
	veml_values.clhr = SI_CLHR;						//this
	veml_values.sclp_en = 1;					//this
	veml_values.sdap_en = 1;				//this
	veml_values.scl_loc = VEML_SCL_LOC;
	veml_values.sda_loc = VEML_SDA_LOC;
	veml_values.event_cb = SI7021_READ_CB;	//this

	i2c_open(I2Cx, &veml_values);

}
