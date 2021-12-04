/**
 * @file gpio.c
 * @author Case Chrisbacher
 * @date March 14th, 2021
 * @brief Enables GPIO and Configures Pins
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"


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
 * 	Enables the GPIO clock and Initializes pins for LED output
 *
 *
 * @details
 *	Enables clocks used in GPIO and then sets the strengths and pinmodes for the LED outputs.
 *
 * @note
 *	Used in app_peripheral_setup. Sets up device before program is run.
 *
 ******************************************************************************/

void gpio_open(void){

	CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

	GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);

	//Configure Li7021 Sensor

	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, SENSOR_DRIVE_STRENGTH);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, gpioModePushPull, SI7021_SENSOR_EN_DEF);

	//Configure Li7021 Pins
	GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, gpioModeWiredAnd, true);
	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, gpioModeWiredAnd, true);

	//Configure VEML6030 pins
	GPIO_PinModeSet(VEML6030_SCL_PORT, VEML6030_SCL_PIN, gpioModeWiredAnd, true);
	GPIO_PinModeSet(VEML6030_SDA_PORT, VEML6030_SDA_PIN, gpioModeWiredAnd, true);

	//Configure LEUART
	//TX
	GPIO_DriveStrengthSet(LEUART_TX_PORT, LEUART_DRIVE_STRENGTH);
	GPIO_PinModeSet(LEUART_TX_PORT, LEUART_TX_PIN, gpioModePushPull, true);
	//RX
	GPIO_PinModeSet(LEUART_RX_PORT, LEUART_RX_PIN, gpioModeInput, true);



}
