//***********************************************************************************
// Include files
//***********************************************************************************

#ifndef BRD_CONFIG_HG
#define BRD_CONFIG_HG
/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_gpio.h"

/* The developer's include statements */


//***********************************************************************************
// defined files
//***********************************************************************************

// GPIO pin setup
#define STRONG_DRIVE

// LED 0 pin is
#define	LED0_PORT				gpioPortF
#define LED0_PIN				04u
#define LED0_DEFAULT			false 	// Default false (0) = off, true (1) = on
#define LED0_GPIOMODE			gpioModePushPull


// LED 1 pin is
#define LED1_PORT				gpioPortF
#define LED1_PIN				05u
#define LED1_DEFAULT			false	// Default false (0) = off, true (1) = on
#define LED1_GPIOMODE			gpioModePushPull

#ifdef STRONG_DRIVE
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
#else
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
#endif


// System Clock setup
#define MCU_HFXO_FREQ			cmuHFRCOFreq_19M0Hz


// LETIMER PWM Configuration

#define		PWM_ROUTE_0			LETIMER_ROUTELOC0_OUT0LOC_LOC28
#define		PWM_ROUTE_1			LETIMER_ROUTELOC0_OUT1LOC_LOC28

//Si7021 Configuration
#define	SI7021_SCL_PORT			gpioPortC
#define	SI7021_SCL_PIN			11u
#define	SI7021_SDA_PORT			gpioPortC
#define	SI7021_SDA_PIN			10u

#define	SI7021_SENSOR_EN_PORT	gpioPortB
#define	SI7021_SENSOR_EN_PIN	10u
#define SI7021_SENSOR_EN_DEF	true

#define SENSOR_DRIVE_STRENGTH	gpioDriveStrengthWeakAlternateWeak

//VEML6030 Configuration
#define	VEML6030_SCL_PORT		gpioPortB
#define	VEML6030_SCL_PIN		12u
#define	VEML6030_SDA_PORT		gpioPortB
#define	VEML6030_SDA_PIN		13u

//LEUART0
#define LEUART_TX_PORT			gpioPortD
#define LEUART_TX_PIN			10u
#define LEUART_RX_PORT			gpioPortD
#define LEUART_RX_PIN			11u

#define LEUART_DRIVE_STRENGTH	gpioDriveStrengthStrongAlternateWeak

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

#endif
