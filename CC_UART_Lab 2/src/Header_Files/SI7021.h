//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	SI7021_HG
#define	SI7021_HG

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
#define SI_REF_FREQ			0
#define SI_FREQ				I2C_FREQ_FAST_MAX
#define SI_CLHR				i2cClockHLRAsymetric

#define I2Cx 				I2C1
#define SI_SCL_LOC			I2C_ROUTELOC0_SCLLOC_LOC19
#define SI_SDA_LOC			I2C_ROUTELOC0_SDALOC_LOC19				//I2C1: LOC19


#define slave_add				0x40
#define humidity_no_hold		0xF5
#define temp_no_hold			0xF3
#define write_user_reg1			0xE6
#define read_user_reg1			0xE7
#define reset_setting			0b00111010
#define res_one_bit				0b00111011


//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// function prototypes
//***********************************************************************************
void si7021_i2c_open(void);
void si7021_read(uint32_t callback);
float si7021_rel_humidity(void);
float si7021_rel_temp(void);
bool si7021_tdd_config(void);

#endif
