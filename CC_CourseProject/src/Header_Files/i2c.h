//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	I2C_HG
#define	I2C_HG

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
#include "sleep_routines.h"
#include "SI7021.h"
#include "veml6030.h"

//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// global variables
//***********************************************************************************
typedef struct {
	bool					enable;
	bool					master;
	uint32_t 				refFreq;
	uint32_t				freq;
	I2C_ClockHLR_TypeDef	clhr;

	bool					sclp_en;
	bool					sdap_en;
	uint32_t				scl_loc;
	uint32_t				sda_loc;

	uint32_t				event_cb;



} I2C_OPEN_STRUCT ;

typedef enum  {
	start_comm,
	command,
	slave_rw,
	MS_byte,
	LS_byte,
	end_comm

}DEFINED_STATES;


typedef struct {
	DEFINED_STATES		state;
	uint32_t			slave_address;
	uint32_t			command_;
	I2C_TypeDef			*I2C_periph;
	uint32_t			*data;
	uint32_t			callback;
	uint32_t			numbytes;
	bool				busy;
	bool				read;


} I2C_STATE_MACHINE ;



//***********************************************************************************
// function prototypes
//***********************************************************************************

void i2c_open(I2C_TypeDef*i2c, I2C_OPEN_STRUCT *i2c_setup);
void i2c_bus_reset(I2C_TypeDef * i2c);
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
void i2c_start(I2C_TypeDef*i2c, uint32_t address_slave, uint32_t command, uint32_t *data, uint32_t callback, uint32_t numbytes, bool busy, bool read);
bool i2c_busy(I2C_TypeDef*i2c);


#endif
