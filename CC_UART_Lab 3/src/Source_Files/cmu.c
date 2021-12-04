/**
 * @file cmu.c
 * @author Case Chrisbacher
 * @date March 14th, 2021
 * @brief Clock Management Unit driver
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

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
 * 	Configure clock tree
 *
 *
 * @details
 *	Enables clocks used in design and disables oscillators which are on by default, but
 *	are not being used for this project.
 *
 * @note
 *	Used in app_peripheral_setup. Sets up device before program is run.
 *
 ******************************************************************************/

void cmu_open(void){

		CMU_ClockEnable(cmuClock_HFPER, true);


		// By default, Low Frequency Resistor Capacitor Oscillator, LFRCO, is enabled,
		// Disable the LFRCO oscillator
		CMU_OscillatorEnable(cmuOsc_LFRCO , false, false);	 // What is the enumeration required for LFRCO?

		// Enable the Low Frequency Crystal Oscillator, LFXO
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);	// What is the enumeration required for LFXO?

		// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

		// Route LF clock to LETIMER0 clock tree
		CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_ULFRCO);	// What clock tree does the LETIMER0 reside on?

		// Now, you must ensure that the global Low Frequency is enabled
		CMU_ClockEnable(cmuClock_CORELE, true);	//This enumeration is found in the Lab 4 assignment

		CMU_ClockSelectSet(cmuClock_LFB , cmuSelect_LFXO);

}

