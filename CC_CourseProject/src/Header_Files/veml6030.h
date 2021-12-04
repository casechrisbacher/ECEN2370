//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	VEML6030_HG
#define	VEML6030_HG

/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"
#include "em_i2c.h"
#include "em_gpio.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "brd_config.h"
#include "i2c.h"
#include "app.h"


//***********************************************************************************
// defined files
//***********************************************************************************
#define I2Cy 				I2C0
#define VEML_SCL_LOC		I2C_ROUTELOC0_SCLLOC_LOC6				//I2C0
#define VEML_SDA_LOC		I2C_ROUTELOC0_SDALOC_LOC8
#define VEML_REF_FREQ		0
#define VEML_FREQ			I2C_FREQ_FAST_MAX
#define VEML_CLHR			i2cClockHLRAsymetric
#define ambient_light		0x0000
#define VEML_slave			0x48

//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// function prototypes
//***********************************************************************************
void veml6030_i2c_open(void);
void veml6030_read(uint32_t callback);
float veml6030_rel_light(void);


#endif /*VEML6030_HG */
