/**
 * @file leuart.c
 * @author Case Chrisbacher
 * @date April 2, 2021
 * @brief Contains all the functions of the LEUART peripheral
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Library includes
#include <string.h>

//** Silicon Labs include files
#include "em_gpio.h"
#include "em_cmu.h"

//** Developer/user include files
#include "leuart.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
uint32_t	rx_done_evt;
uint32_t	tx_done_evt;
bool		leuart0_tx_busy;
static LEUART_STATE_MACHINE leuart_sm;

/***************************************************************************//**
 * @brief LEUART driver
 * @details
 *  This module contains all the functions to support the driver's state
 *  machine to transmit a string of data across the LEUART bus.  There are
 *  additional functions to support the Test Driven Development test that
 *  is used to validate the basic set up of the LEUART peripheral.  The
 *  TDD test for this class assumes that the LEUART is connected to the HM-18
 *  BLE module.  These TDD support functions could be used for any TDD test
 *  to validate the correct setup of the LEUART.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************
static void leuart_txbl(LEUART_STATE_MACHINE *leuart_state);
static void leuart_txc(LEUART_STATE_MACHINE *leuart_state);


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Opens transmission of LEUART
 *
 * @details
 *  Begins by enabling the clock to the LEUART0 peripheral, which is the only LEUART we will use, so if it is
 *  not that then it hits the EFM_ASSERT. then it tests to make sure that the clock is configured correctly.
 *  A local leuart typedef is created, pins are routed and then the leuart is enabled.
 *  The while statement stalls until this is finished
 *
 * @param[in] leuart
 * 	Will only ever be LEAURT0
 *
 * 	@param[in] leuart_settings
 * 	 Previously defined struct with the wanted leuart settings. For this project, ble_open defines all of the values to enable bluetooth communication.
 *
 ******************************************************************************/

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){
	if(leuart == LEUART0){
		CMU_ClockEnable(cmuClock_LEUART0, true);			//Enables clock to appropriate LEUART peripheral
	}
	else{
		EFM_ASSERT(false);					//Should not enter
	}

	if((leuart->STARTFRAME & 0x01) == 0) {
		leuart->STARTFRAME = 0x01;
		EFM_ASSERT(leuart->STARTFRAME & 0x01);
		leuart->STARTFRAME= 0x01;
	} else{
		leuart->STARTFRAME= 0x01;
		EFM_ASSERT(!(leuart->STARTFRAME & 0x01));
	}

	LEUART_Init_TypeDef leuart_init;
	leuart_init.baudrate = leuart_settings->baudrate;
	leuart_init.databits = leuart_settings->databits;
	leuart_init.enable = leuart_settings->enable;
	leuart_init.parity = leuart_settings->parity;
	leuart_init.stopbits = leuart_settings->stopbits;
	leuart_init.refFreq = leuart_settings->refFreq;
	LEUART_Init(leuart, &leuart_init);
	while(leuart->SYNCBUSY);

	leuart->ROUTELOC0 = leuart_settings->rx_loc | leuart_settings->tx_loc;
	leuart->ROUTEPEN = (leuart_settings->rx_pin_en *LEUART_ROUTEPEN_RXPEN) | (leuart_settings->tx_pin_en *LEUART_ROUTEPEN_TXPEN);


	leuart_cmd_write(leuart, LEUART_CMD_CLEARTX);
	leuart_cmd_write(leuart, LEUART_CMD_CLEARRX);

	LEUART_Enable(leuart, leuart_init.enable);
	while(leuart->SYNCBUSY);

	while(!(leuart->STATUS & (LEUART_STATUS_TXENS | LEUART_STATUS_RXENS)));
	EFM_ASSERT(leuart->STATUS & LEUART_STATUS_TXENS);
	EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXENS);

	if(leuart == LEUART0){
		NVIC_EnableIRQ(LEUART0_IRQn);
	}
	else{
		EFM_ASSERT(false);					//Should not enter
	}
}

/***************************************************************************//**
 * @brief
 *  Interrupt Handler for LEUART0
 *
 * @details
 * 	If statements are triggered if the TXBL or TXC interrupt flags are triggered.
 *
 ******************************************************************************/

void LEUART0_IRQHandler(void){
	uint32_t int_flag = LEUART0->IF & LEUART0->IEN;
	LEUART0->IFC = int_flag;

	if(int_flag & LEUART_IF_TXBL){
		leuart_txbl(&leuart_sm);
	}
	if(int_flag & LEUART_IF_TXC){
		leuart_txc(&leuart_sm);
	}

}


/***************************************************************************//**
 * @brief
 *  Interrupt Function for TXBL
 *
 * @details
 * 	When the TXBL interrupt is triggered, the IRQ calls this function. Transmit is the only state that this
 * 	interrupt should be called. Transmit sends a letter and increases the bytes until the bytes sent is equal to the
 * 	length minus 1. At this point TXC is enabled and TXBL is disabled and it changes the state to the
 * 	final state "stop". This interrupt is called over and over until full word is sent.
 *
 * 	@param[in] *leuart_state
 * 		Leuart struct that contains all of the variables to transmit data
 *
 ******************************************************************************/

void leuart_txbl(LEUART_STATE_MACHINE *leuart_state){
	switch(leuart_state->state){
	case start:
		EFM_ASSERT(false);
		break;
	case transmit:
		leuart_app_transmit_byte(leuart_state->LEUART_periph, leuart_state->string_[leuart_state->sent_bytes]);
		leuart_state->sent_bytes++;
		if(leuart_state->sent_bytes == leuart_state->string_length - 1){
			leuart_state->LEUART_periph->IEN |= LEUART_IF_TXC;
			leuart_state->LEUART_periph->IEN &= ~LEUART_IF_TXBL;
			leuart_state->state = stop;
		}
		break;
	case stop:
		EFM_ASSERT(false);
		break;
	default:
		EFM_ASSERT(false);
		break;
	}
}

/***************************************************************************//**
 * @brief
 *  Interrupt Function for TXC
 *
 * @details
 * 	When the TXC interrupt is triggered, it should only be in the stop state. This stop state disables TXC,
 * 	says that the leuart isn't busy, unblocks sleep, and triggers the transmission event.
 *
 * 	@param[in] *leuart_state
 * 		Leuart struct that contains all of the variables to transmit data
 *
 ******************************************************************************/

void leuart_txc(LEUART_STATE_MACHINE *leuart_state){
	switch(leuart_state->state){
	case start:
		EFM_ASSERT(false);
		break;

	case transmit:
		EFM_ASSERT(false);
		break;

	case stop:
		leuart_state->LEUART_periph->IEN &= ~LEUART_IF_TXC;
		leuart_state->busy = false;
		sleep_unblock_mode(LEUART_TX_EM);
		add_scheduled_event(BLE_TX_DONE_CB);
		break;
	default:
		EFM_ASSERT(false);
		break;
	}
}

/***************************************************************************//**
 * @brief
 * 	Begin Transmission using LEUART
 *
 * @breif
 * 	This function stalls while the leuart is busy. It then initializes a local state machine variable.
 * 	It also does the first few steps in transmitting data by blocking the sleep mode and enabling the
 * 	TXBL interrupt
 *
 * @param[in] *leuart
 * 	Pointer to LEUART peripheral
 *
 * @param[in] *string
 * 	String which is being transmitted
 *
 * @param[in] call_back
 *  Transmission callback
 *
 ******************************************************************************/

void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t call_back){
	while(leuart_tx_busy(leuart));

	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	leuart_sm.sent_bytes = 0;
	leuart_sm.LEUART_periph = leuart;
	strcpy(leuart_sm.string_, string);
	leuart_sm.string_length = strlen(string);
	leuart_sm.busy = true;
	leuart_sm.state = transmit;
	leuart_sm.callback = call_back;

	sleep_block_mode(LEUART_TX_EM);
	leuart->IEN |= LEUART_IEN_TXBL;

	CORE_EXIT_CRITICAL();

}

/***************************************************************************//**
 * @brief
 * 	Checks if leuart is busy
 *
 * @details
 * 	Returns the busy state of peripheral
 *
 * @return
 *  Busy or not
 *
 ******************************************************************************/

bool leuart_tx_busy(LEUART_TypeDef *leuart){
	return leuart_sm.busy;
}

/***************************************************************************//**
 * @brief
 *   LEUART STATUS function returns the STATUS of the peripheral for the
 *   TDD test
 *
 * @details
 * 	 This function enables the LEUART STATUS register to be provided to
 * 	 a function outside this .c module.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the STATUS register value as an uint32_t value
 *
 ******************************************************************************/

uint32_t leuart_status(LEUART_TypeDef *leuart){
	uint32_t	status_reg;
	status_reg = leuart->STATUS;
	return status_reg;
}

/***************************************************************************//**
 * @brief
 *   LEUART CMD Write sends a command to the CMD register
 *
 * @details
 * 	 This function is used by the TDD test function to program the LEUART
 * 	 for the TDD tests.
 *
 * @note
 *   Before exiting this function to update  the CMD register, it must
 *   perform a SYNCBUSY while loop to ensure that the CMD has by synchronized
 *   to the lower frequency LEUART domain.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] cmd_update
 * 	 The value to write into the CMD register
 *
 ******************************************************************************/

void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update){

	leuart->CMD = cmd_update;
	while(leuart->SYNCBUSY);
}

/***************************************************************************//**
 * @brief
 *   LEUART IF Reset resets all interrupt flag bits that can be cleared
 *   through the Interrupt Flag Clear register
 *
 * @details
 * 	 This function is used by the TDD test program to clear interrupts before
 * 	 the TDD tests and to reset the LEUART interrupts before the TDD
 * 	 exits
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 ******************************************************************************/

void leuart_if_reset(LEUART_TypeDef *leuart){
	leuart->IFC = 0xffffffff;
}

/***************************************************************************//**
 * @brief
 *   LEUART App Transmit Byte transmits a byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a transmit byte, a while statement checking for the TXBL
 *   bit in the Interrupt Flag register is required before writing the
 *   TXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] data_out
 *   Byte to be transmitted by the LEUART peripheral
 *
 ******************************************************************************/

void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out){
	while (!(leuart->IF & LEUART_IF_TXBL));
	leuart->TXDATA = data_out;
}


/***************************************************************************//**
 * @brief
 *   LEUART App Receive Byte polls a receive byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a receive byte, a while statement checking for the RXDATAV
 *   bit in the Interrupt Flag register is required before reading the
 *   RXDATA register.
 *
 * @param[in] leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the byte read from the LEUART peripheral
 *
 ******************************************************************************/

uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart){
	uint8_t leuart_data;
	while (!(leuart->IF & LEUART_IF_RXDATAV));
	leuart_data = leuart->RXDATA;
	return leuart_data;
}
