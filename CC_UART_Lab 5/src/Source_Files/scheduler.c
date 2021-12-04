/**
 * @file scheduler.c
 * @author Case Chrisbacher
 * @date March 14th, 2021
 * @brief Scheduler Functions
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "scheduler.h"
#include "em_assert.h"
#include "em_core.h"
#include "em_emu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************
static unsigned int event_scheduled;

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * 	Initially opens scheduler.
 *
 *
 * @details
 *	Interrupts will not interrupt the execution of the code because of the first two statements.
 *	Sets private variable event_scheduled to 0
 *	Final statement re-enables interrupts
 *
 * @note
 *
 *
 ******************************************************************************/
void scheduler_open(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled = 0;
	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * 	Adds event to scheduler.
 *
 *
 * @details
 *	Interrupts will not interrupt the execution of the code because of the first two statements.
 *	Or's new event with static variable event_scheduled
 *	Final statement re-enables interrupts
 *
 * @param[in] event
 *   New event to be added to scheduler
 *
 * @note
 *
 *
 ******************************************************************************/

void add_scheduled_event(uint32_t event){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled |= event;
	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * 	Removes event to scheduler.
 *
 *
 * @details
 *	Interrupts will not interrupt the execution of the code because of the first two statements.
 *	Negates bits from the static variable event_scheduled
 *	Final statement re-enables interrupts
 *
 * @param[in] event
 *   New event to be removed scheduler
 *
 * @note
 *
 *
 ******************************************************************************/

void remove_scheduled_event(uint32_t event){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled &= ~event;
	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * 	Returns scheduled event.
 *
 *
 * @details
 *	Returns the value of the static event_scheduled
 *
 *
 * @note
 *	Used to pass to different areas
 *
 ******************************************************************************/

uint32_t get_scheduled_events(void){
	return event_scheduled;

}

