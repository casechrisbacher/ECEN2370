/**
 * @file SI7021.c
 * @author Case Chrisbacher
 * @date March 14th, 2021
 * @brief Si7021 Driver
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "SI7021.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t data_h;
static uint32_t data_t;
static uint32_t data;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  Opens I2C specifically for the Si7021
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
void si7021_i2c_open(){
	I2C_OPEN_STRUCT si_values;
	si_values.enable = 1;
	si_values.master = 1;
	si_values.refFreq = SI_REF_FREQ;
	si_values.freq = SI_FREQ;
	si_values.clhr = SI_CLHR;
	si_values.sclp_en = 1;				//pulls low
	si_values.sdap_en = 1;
	si_values.scl_loc = SI_SCL_LOC;
	si_values.sda_loc = SI_SDA_LOC;
	si_values.event_cb = SI7021_READ_CB;

	i2c_open(I2Cx, &si_values);

}

/***************************************************************************//**
 * @brief
 *  Reads Si7021
 *
 *
 * @details
 *	Sole purpose is to call and provide the parameters to start the i2c for the si7021.
 *
 * @note
 *
 * @param[in] callback
 * 	Call back value for the scheduler.
 *
 *
 ******************************************************************************/

void si7021_read(uint32_t callback){
	if(callback == SI7021_READ_CB){
		i2c_start(I2Cx, slave_add , humidity_no_hold, &data_h, callback, 2, true, true);
	}
	else if (callback == SI7021_READ_CB_T){
		i2c_start(I2Cx, slave_add , temp_no_hold, &data_t, callback, 2, true, true);
	}
}

/***************************************************************************//**
 * @brief
 *  Humidity Value Conversion Function
 *
 *
 * @details
 *	Function takes the local value of read in the si7021 device and converts it to the relative humidity value.
 *	This is then rounded to one decimal place.
 *
 * @note
 * Returns the float value of the percent
 *
 ******************************************************************************/

float si7021_rel_humidity(){
	float percent = 0;//relative humidity
	percent = ((125.0*data_h)/(65536)) - 6;		//Formula given in si7021 Humidity and Temp Manual
	percent = (float)((int)(percent*10))/10;				//Rounds to one decimal place
	return percent;

}

float si7021_rel_temp(){
	float temp = 0;
	temp = ((175.72*data_t)/(65536)) -46.85;
	temp = (temp*(9.0/5.0)) +32.0;					//Celsius to Fahrenheit
	temp = (float)((int)(temp*10))/10;				//Rounds to One decimal
	return temp;
}

/***************************************************************************//**
 * @brief
 *  Test Driven Development for I2C
 *
 *
 * @details
 *	1) Test Brief: Test goes through the reset one bit value read, one bit value write/ read, and then calls
 *	i2c_start to make sure that the humidity sensor is not broken.
 *
 *	2) Functionality Tested: This function tests the write and read functionality of the i2c state machine.
 *	It initially checks to make sure that the first read value is the reset value. For the second test it writes
 *	to the data pointer and then reads it and makes sure it is not the same as it was before. The third section calls
 *	our humidity with no hold command, sensing the humidity and confirming functionality.
 *
 *	3) Test Escapes: The test will terminate if any of the three blocks are not successful. If not, the
 *	test will go through (with some delays) and return a true bool.
 *
 ******************************************************************************/
bool si7021_tdd_config(void){
	bool complete;
	timer_delay(80);	//Hardware Timer Delay

	//Read Si7021 User 1 Register, stall until reading done, check value
	i2c_start(I2Cx, slave_add, read_user_reg1, &data, 0b0000, 1, true, true);
	while(i2c_busy());
	EFM_ASSERT(data == reset_setting || data == res_one_bit);

	//Set to new value, stall, hardware delay, write new value, check value is not the same
	data = res_one_bit;

	i2c_start(I2Cx, slave_add , write_user_reg1, &data, 0b0000, 1, true, false);				//THIS ALWAYS sets event scheduled to Si CB
	while(i2c_busy());									//^^ This is the callback input
	timer_delay(80);
	i2c_start(I2Cx, slave_add , read_user_reg1, &data, 0b0000, 1, true, true);
	while(i2c_busy());
	EFM_ASSERT(data == res_one_bit);

	//Real Humidity, stall, check if reading is correct
	i2c_start(I2Cx, slave_add , humidity_no_hold, &data, 0b0000, 2, true, true);
	while(i2c_busy());
	if(data > 40 || data < 10){
		EFM_ASSERT(false);
	}

	//If it passes all sections the test returns true
	complete = true;
	return complete;

}






