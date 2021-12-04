//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	APP_HG
#define	APP_HG

/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "scheduler.h"
#include "letimer.h"
#include "brd_config.h"
#include "si7021.h"
#include "ble.h"
#include "HW_delay.h"


//***********************************************************************************
// defined files
//***********************************************************************************
#define		SYSTEM_BLOCK_EM		EM3
#define		PWM_PER				1.8		// PWM period in seconds
#define		PWM_ACT_PER			0.25	// PWM active period in seconds
#define 	PWM_ROUTE_0			LETIMER_ROUTELOC0_OUT0LOC_LOC28
#define		PWM_ROUTE_1			LETIMER_ROUTELOC0_OUT1LOC_LOC28
#define		LETIMER0_COMP0_CB	0x00000001	//0b00000001		//all represented by unique bits
#define		LETIMER0_COMP1_CB	0x00000002	//0b00000010
#define		LETIMER0_UF_CB		0x00000004	//0b00000100
#define		SI7021_REG1_CB		0x00000008	//0b00001000
#define		BOOT_UP_CB			0x00000010	//0b00010000
#define		BLE_TX_DONE_CB		0x00000020
#define		BLE_RX_DONE_CB		0x00000040
#define		SI7021_READ_CB		0x00000080
#define		SI7021_READ_CB_T	0x00000100



//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void scheduled_letimer0_uf_cb(void);
void scheduled_letimer0_comp0_cb(void);
void scheduled_letimer0_comp1_cb(void);
void service_si7021_humidity_done(void);
void service_si7021_temp_done(void);
void scheduled_boot_up_cb(void);
void scheduled_ble_tx_cb(void);

#endif
