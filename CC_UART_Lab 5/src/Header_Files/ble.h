/*
 * ble.h
 *
 *  Created on:
 *      Author: Case Chrisbacher
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	BLE_GUARD_H
#define	BLE_GUARD_H

//** Standard Libraries
#include <stdbool.h>
#include <stdint.h>

// Driver functions
#include "leuart.h"
#include "gpio.h"


//***********************************************************************************
// defined files
//***********************************************************************************

#define HM18_LEUART0		LEUART0
#define HM18_BAUDRATE		9600
#define	HM18_DATABITS		leuartDatabits8
#define HM18_ENABLE			leuartEnable
#define HM18_PARITY			leuartNoParity
#define HM18_REFFREQ		0					// use reference clock
#define HM18_STOPBITS		leuartStopbits1

#define LEUART0_TX_ROUTE	LEUART_ROUTELOC0_TXLOC_LOC18   	// Route to ...
#define LEUART0_RX_ROUTE	LEUART_ROUTELOC0_RXLOC_LOC18   	// Route to ...

#define CIRC_TEST			true
#define	CIRC_OPER			false
#define	CSIZE				64
#define CIRC_TEST_SIZE		3

typedef struct {
	char			cbuf[CSIZE];
	uint8_t 		size_mask;
	uint32_t 		size;
	uint32_t 		read_ptr;
	uint32_t 		write_ptr;
} BLE_CIRCULAR_BUF;

typedef struct {
	char			test_str[CIRC_TEST_SIZE][CSIZE];
	char			result_str[CSIZE];
}CIRC_TEST_STRUCT;




//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void ble_open(uint32_t tx_event, uint32_t rx_event);
void ble_write(char *string, uint32_t tx_call_back_done);
bool ble_test(char *mod_name);
void circular_buff_test(void);
bool ble_circ_pop(bool test);


#endif
