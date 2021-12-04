/**************************************************************************
* @file sleep_routines.c
***************************************************************************
* @section License
* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
***************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
* @author Case Chrisbacher
* @date March 14th, 2021
* @brief Sleep Functions
 *
**************************************************************************/

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files

//** User/developer include files
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// Private variables
//***********************************************************************************
static int lowest_energy_mode[MAX_ENERGY_MODES];

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * 	Initializes sleep routines.
 *
 *
 * @details
 *	Runs a for loop to initialize the private variable lowest_energy_mode[]
 *
 *
 * @note
 *	No return
 *
 ******************************************************************************/
void sleep_open(void){
	for(uint32_t i = 0; i < MAX_ENERGY_MODES; i++){
		lowest_energy_mode[i] = 0;
	}
}

/***************************************************************************//**
 * @brief
 * 	Used to enter sleep.
 *
 *
 * @details
 *	Interrupts will not interrupt the execution of the code because of the first two statements.
 *	Goes through the lowest energy mode array and finds the lowest energy mode that
 *	it can handle and then goes into that energy mode.
 *	Final statement re-enables interrupts
 *
 *
 * @note
 *
 ******************************************************************************/

void enter_sleep(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	if(lowest_energy_mode[EM0] > 0){
	}
	else if(lowest_energy_mode[EM1] > 0){
	}
	else if(lowest_energy_mode[EM2] > 0){
			EMU_EnterEM1();
		}
	else if(lowest_energy_mode[EM3] > 0){
			EMU_EnterEM2(true);
		}
	else{
		EMU_EnterEM3(true);

	}
	CORE_EXIT_CRITICAL();
	return;
}

/***************************************************************************//**
 * @brief
 * 	Finds energy mode which system cannot enter
 *
 *
 * @details
 *	Goes through the lowest_energy_mode array until it finds a non zero value.
 *	This value is one above the value in which the max energy mode can be.
 *	The mode below this is then returned
 *
 *
 * @note
 *
 ******************************************************************************/
uint32_t current_block_energy_mode(void){
	for(uint32_t i = 0; i < MAX_ENERGY_MODES; i++){
		if(lowest_energy_mode[i] != 0){
			return i;
		}
	}
	return(MAX_ENERGY_MODES - 1);
}

/***************************************************************************//**
 * @brief
 * 	Releases the processor from going into a sleep mode with a peripheral that is not longer active
 *
 *
 * @details
 *	Interrupts will not interrupt the execution of the code because of the first two statements.
 *	Removes a value from the lowest_energy_mode array
 *	Final statement re-enables interrupts
 *
 * @param[in] EM
 *   Location of energy mode to be unblocked
 *
 * @note
 *
 *
 ******************************************************************************/

void sleep_unblock_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	if(lowest_energy_mode[EM] > 0){
		lowest_energy_mode[EM]--;
	}

	CORE_EXIT_CRITICAL();

	EFM_ASSERT(lowest_energy_mode[EM] >= 0);
}

/***************************************************************************//**
 * @brief
 * 	Releases the processor from going into a sleep mode with a peripheral is active
 *
 *
 * @details
 *	Interrupts will not interrupt the execution of the code because of the first two statements.
 *	Adds a value from the lowest_energy_mode array
 *	Final statement re-enables interrupts
 *
 * @param[in] EM
 *   Location of energy mode to be blocked
 *
 * @note
 *
 *
 ******************************************************************************/
void sleep_block_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	lowest_energy_mode[EM]++;

	CORE_EXIT_CRITICAL();
	EFM_ASSERT(lowest_energy_mode[EM] < 5);
}
