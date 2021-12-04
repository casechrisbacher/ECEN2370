/**
 * @file i2c.c
 * @author Case Chrisbacher
 * @date March 14th, 2021
 * @brief I2C
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c.h"

/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "brd_config.h"


//***********************************************************************************
// defined files
//***********************************************************************************
#define WRITE 		0
#define READ		1

//***********************************************************************************
// Private variables
//***********************************************************************************
static I2C_STATE_MACHINE i2c_sm;
static uint32_t event;



//***********************************************************************************
// Private functions
//***********************************************************************************

static void i2c_ack(I2C_STATE_MACHINE *i2c_state_m);
static void i2c_nack(I2C_STATE_MACHINE *i2c_state_m);
static void i2c_rxdatav(I2C_STATE_MACHINE *i2c_state_m);
static void i2c_mstop(I2C_STATE_MACHINE *i2c_state_m);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Starts the I2C
 *
 * @details
 *	This function goes through all of the steps to start the I2C. First it enables the cock to the correct I2C peripheral. Then there is a test to
 *	verify proper I2C clock operation. Then a struct is initialized to provide a base address to configure the I2C. The initialize function is called and all
 *	the pins and ports for the are routed. The function then resets the bus, enables the interrupts, and enables the interrupts at the CPU level with NVIC_Enable
 *
 *
 * @note
 *	Function uses if/ else statements to allow use of both I2C0 and I2C1
 *
 * @param[in] *i2c
 *	Pointer to i2c peripheral.
 *
 * @param[in] *i2c_setup
 * 	Pointer to struct used to initialize local struct and then initialize the I2C.
 *
 ******************************************************************************/

void i2c_open(I2C_TypeDef*i2c, I2C_OPEN_STRUCT *i2c_setup){
	if(i2c == I2C0){
		CMU_ClockEnable(cmuClock_I2C0, true);					//Enables clock to appropriate I2C peripheral
	}
	else if(i2c == I2C1){
		CMU_ClockEnable(cmuClock_I2C1, true);
	}

	if((i2c->IF & 0x01) == 0) {
		i2c->IFS = 0x01;
		EFM_ASSERT(i2c->IF & 0x01);
		i2c->IFC= 0x01;
	} else{
		i2c->IFC= 0x01;
		EFM_ASSERT(!(i2c->IF& 0x01));
	}

	I2C_Init_TypeDef i2c_init;
	i2c_init.master = i2c_setup->master;
	i2c_init.enable = true;
	i2c_init.refFreq = i2c_setup->refFreq;
	i2c_init.freq = i2c_setup->freq;
	i2c_init.clhr = i2c_setup->clhr;

	I2C_Init(i2c, &i2c_init);


	i2c->ROUTELOC0 = i2c_setup->scl_loc | i2c_setup->sda_loc;
	i2c->ROUTEPEN = (i2c_setup->sdap_en *I2C_ROUTEPEN_SDAPEN) | (i2c_setup->sclp_en * I2C_ROUTEPEN_SCLPEN);

	i2c_bus_reset(i2c);

	i2c->IFC = i2c->IF;
	i2c->IEN = 0;
	i2c->IEN |= I2C_IF_ACK;
	i2c->IEN |= I2C_IF_NACK;
	i2c->IEN |= I2C_IF_RXDATAV;
	i2c->IEN |= I2C_IF_MSTOP;

	event = i2c_setup->event_cb;

	if(i2c == I2C0){
			NVIC_EnableIRQ(I2C0_IRQn);
		}
	else if(i2c == I2C1){
			NVIC_EnableIRQ(I2C1_IRQn);
		}

}

/***************************************************************************//**
 * @brief
 *	Resets the Bus
 *
 * @details
 *	This function resets the I2C state machine of the Pearl Gecko, along with the I2C state machines of the external device connected
 *
 *
 * @note
 * 	For this lab the SI7021 I2C state machine peripherals are being reset
 *
 * @param[in] *i2c
 *	Pointer to i2c peripheral.
 *
 ******************************************************************************/

void i2c_bus_reset(I2C_TypeDef * i2c){
	uint32_t IEN_state;
	if(i2c->STATE & I2C_STATE_BUSY){
		i2c->CMD = I2C_CMD_ABORT;
		while(i2c->STATE & I2C_STATE_BUSY);
	}

	IEN_state = i2c->IEN;							//saves state in local variable
	i2c->IEN = 0;									//Disables interrupts
	i2c->IFC |= i2c->IF;							//Clear interrupt flag
	i2c->CMD |= I2C_CMD_CLEARTX;					//Clear transfer buffer
	i2c->CMD |= I2C_CMD_STOP | I2C_CMD_START;		//Set start and stop
	while(!(i2c->IF & I2C_IF_MSTOP));				//stall
	i2c->IFC |= i2c->IF;
	i2c->IEN = IEN_state;							//reset state
	i2c->CMD = I2C_CMD_ABORT;

}

/***************************************************************************//**
 * @brief
 *	Starts the I2C
 *
 * @details
 *	Function initializes static struct with input arguments and then begins the interrupt driven state machine. It also sets the first state and performs the
 *	initialization steps
 *
 *
 * @note
 * 	Write is defined as false
 *
 * @param[in] *i2c
 *	Pointer to i2c peripheral.
 *
 * @param[in] address_slave
 *	Address of the device which the Pearl Gecko controlling.
 *
 * @param[in] command
 *	Command to be sent to the slave.
 *
 * @param[in] *data
 *	Static variable declared in external file, used to keep the value being measured by slave, sent back to the master
 *
 * @param[in] callback
 *	Call back for the scheduler created for the I2C.
 *
 ******************************************************************************/

void i2c_start(I2C_TypeDef*i2c, uint32_t address_slave, uint32_t command, uint32_t *data_, uint32_t callback, uint32_t numbytes, bool busy, bool read){
	EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);
	sleep_block_mode(EM2);


	i2c_sm.I2C_periph = i2c;
	i2c_sm.slave_address = address_slave;
	i2c_sm.command_ = command;
	i2c_sm.data = data_;
	i2c_sm.callback = callback;
	i2c_sm.numbytes = numbytes;
	i2c_sm.busy = busy;
	i2c_sm.read = read;						//Read is true, Write is false

	i2c_sm.state = start_comm;
	i2c->CMD = I2C_CMD_START;
	i2c->TXDATA = (address_slave << 1) | WRITE;


}

/***************************************************************************//**
 * @brief
 *	I2C0 Interrupt Handler
 *
 * @details
 * 	Consisting of a bunch of if statements, if an interrupt is triggered it enters the if statement and calls the local function for that interrupt
 *
 *
 * @note
 * 	Separate Handler called if I2C1 is enabled
 *
 ******************************************************************************/

void I2C0_IRQHandler(){
	uint32_t int_flag = I2C0->IF & I2C0->IEN;
	I2C0->IFC = int_flag;

	if(int_flag & I2C_IF_ACK){
		i2c_ack(&i2c_sm);
	}
	if(int_flag & I2C_IF_NACK){
		i2c_nack(&i2c_sm);
	}
	if(int_flag & I2C_IF_RXDATAV){
		i2c_rxdatav(&i2c_sm);
	}
	if(int_flag & I2C_IF_MSTOP){
		i2c_mstop(&i2c_sm);
	}


}

/***************************************************************************//**
 * @brief
 *	I2C1 Interrupt Handler
 *
 * @details
 * 	Consisting of a bunch of if statements, if an interrupt is triggered it enters the if statement and calls the local function for that interrupt
 *
 *
 * @note
 * 	Separate Handler called if I2C0 is enabled
 *
 ******************************************************************************/

void I2C1_IRQHandler(){
	uint32_t int_flag = I2C1->IF & I2C1->IEN;
	I2C1->IFC = int_flag;

	if(int_flag & I2C_IF_ACK){
		i2c_ack(&i2c_sm);
	}
	if(int_flag & I2C_IF_NACK){
		i2c_nack(&i2c_sm);
	}
	if(int_flag & I2C_IF_RXDATAV){
		i2c_rxdatav(&i2c_sm);
	}
	if(int_flag & I2C_IF_MSTOP){
		i2c_mstop(&i2c_sm);
	}


}


/***************************************************************************//**
 * @brief
 *	ACK interrupt Function
 *
 * @details
 * 	When an ack interrupt is triggered, this function is called in the interrupt handler. Depending on which state the machine is in, it performs a different action.
 * 	All states then set the static state to the next state. If this interrupt is received within a state which it should not be in, an EFM Assert is called.
 *
 * @note
 *
 * @param[in] *i2c_state_m
 *	Private pointer to I2C_STATE_MACHINE associated with the interrupt being serviced.
 *
 ******************************************************************************/

void i2c_ack(I2C_STATE_MACHINE *i2c_state_m){
		if(i2c_state_m->read){
			switch(i2c_state_m->state){
					case start_comm:
						i2c_state_m->state = command;
						i2c_state_m->I2C_periph->TXDATA = i2c_state_m->command_;
						break;
					case command:
						i2c_state_m->state = slave_rw;
						i2c_state_m->I2C_periph->CMD = I2C_CMD_START;
						i2c_state_m->I2C_periph->TXDATA = (i2c_state_m->slave_address << 1) | READ;
						break;
					case slave_rw:
						i2c_state_m->state = MS_byte;
						break;
					case MS_byte:
						EFM_ASSERT(false);
						break;
					case LS_byte:
						EFM_ASSERT(false);
						break;
					case end_comm:
						EFM_ASSERT(false);
							break;
					default:
						EFM_ASSERT(false);
						break;
			}
		}
			else{
				switch(i2c_state_m->state){
					case start_comm:
						i2c_state_m->state = slave_rw;
						i2c_state_m->I2C_periph->TXDATA = i2c_state_m->command_;
						break;
					case command:
						EFM_ASSERT(false);
						break;
					case slave_rw:
						//write data
						i2c_state_m->I2C_periph->TXDATA = *i2c_state_m->data;
						i2c_state_m->numbytes--;

						if(i2c_state_m->numbytes <= 0){				//supporting multiple byte writes
							i2c_state_m->state = end_comm;
						}
						break;
					case MS_byte:
						EFM_ASSERT(false);
						break;
					case LS_byte:
						EFM_ASSERT(false);
						break;
					case end_comm:
						sleep_unblock_mode(EM2);
						add_scheduled_event(i2c_state_m->callback);
						i2c_state_m->state = start_comm;
						i2c_state_m->busy = false;
						break;
					default:
						EFM_ASSERT(false);
						break;
				}
			}

}

/***************************************************************************//**
 * @brief
 *	NACK interrupt Function
 *
 * @details
 * 	When an nack interrupt is triggered, this function is called in the interrupt handler. Depending on which state the machine is in, it performs a different action.
 * 	If this interrupt is received within a state which it should not be in, an EFM Assert is called.
 *
 * @note
 *	The nack calls a repeated start, so the state is reset to the same state which it is in.
 *
 * @param[in] *i2c_state_m
 *	Private pointer to I2C_STATE_MACHINE associated with the interrupt being serviced.
 *
 ******************************************************************************/

void i2c_nack(I2C_STATE_MACHINE *i2c_state_m){
			if(i2c_state_m->read){
				switch(i2c_state_m->state){
				case start_comm:
							EFM_ASSERT(false);
							break;
						case command:
							EFM_ASSERT(false);
							break;
						case slave_rw:
							i2c_state_m->state = slave_rw;
							i2c_state_m->I2C_periph->CMD = I2C_CMD_START;
							i2c_state_m->I2C_periph->TXDATA = (i2c_state_m->slave_address << 1) | READ;
							break;
						case MS_byte:
							EFM_ASSERT(false);
							break;
						case LS_byte:
							EFM_ASSERT(false);
							break;
						case end_comm:
							EFM_ASSERT(false);
							break;
						default:
							EFM_ASSERT(false);
							break;
				}
			}
			else{
				EFM_ASSERT(false);
			}
}

/***************************************************************************//**
 * @brief
 *	RXDATA interrupt Function
 *
 * @details
 * 	When an ack interrupt is triggered, this function is called in the interrupt handler. Depending on which state the machine is in, it performs a different action.
 * 	All states then set the static state to the next state. If this interrupt is received within a state which it should not be in, an EFM Assert is called.
 *
 * @note
 *	RXDATA transmits the data from the slave to the data pointer which is later read by external function. First data read uses equal instead of a pipe to avoid
 *	ever increasing value read.
 *
 * @param[in] *i2c_state_m
 *	Private pointer to I2C_STATE_MACHINE associated with the interrupt being serviced.
 *
 ******************************************************************************/

void i2c_rxdatav(I2C_STATE_MACHINE *i2c_state_m){
		if(i2c_state_m->read){
			switch(i2c_state_m->state){
			case start_comm:
						EFM_ASSERT(false);
						break;
					case command:
						EFM_ASSERT(false);
						break;
					case slave_rw:
						EFM_ASSERT(false);
						break;
					case MS_byte:
						if(i2c_state_m->numbytes == 2){
							i2c_state_m->state = LS_byte;
							*i2c_state_m->data =  (i2c_state_m->I2C_periph->RXDATA << 8);
							i2c_state_m->I2C_periph->CMD = I2C_CMD_ACK;
						}
						else if(i2c_state_m->numbytes == 1){
							i2c_state_m->state = end_comm;
							*i2c_state_m->data =  (i2c_state_m->I2C_periph->RXDATA);
							i2c_state_m->I2C_periph->CMD = I2C_CMD_NACK;
							i2c_state_m->I2C_periph->CMD = I2C_CMD_STOP;
						}
						break;
					case LS_byte:
						i2c_state_m->state = end_comm;
						*i2c_state_m->data |= (i2c_state_m->I2C_periph->RXDATA);
						i2c_state_m->I2C_periph->CMD = I2C_CMD_NACK;
						i2c_state_m->I2C_periph->CMD = I2C_CMD_STOP;
						break;
					case end_comm:
						EFM_ASSERT(false);
						break;
					default:
						EFM_ASSERT(false);
						break;
			}
		}
		else{
				EFM_ASSERT(false);
		}

}

/***************************************************************************//**
 * @brief
 *	MSTOP interrupt Function
 *
 * @details
 * 	When an ack interrupt is triggered, this function is called in the interrupt handler. Depending on which state the machine is in, it performs a different action.
 * 	All states then set the static state to the next state. If this interrupt is received within a state which it should not be in, an EFM Assert is called.
 *
 * @note
 *	Final function in state machine. Resets the state to first state, adds event, and unblocks sleep on device
 *
 * @param[in] *i2c_state_m
 *	Private pointer to I2C_STATE_MACHINE associated with the interrupt being serviced.
 *
 ******************************************************************************/

void i2c_mstop(I2C_STATE_MACHINE *i2c_state_m){
	if(i2c_state_m->read){
		switch(i2c_state_m->state){
			case start_comm:
				EFM_ASSERT(false);
				break;
			case command:
				EFM_ASSERT(false);
				break;
			case slave_rw:
				EFM_ASSERT(false);
				break;
			case MS_byte:
				EFM_ASSERT(false);
				break;
			case LS_byte:
				EFM_ASSERT(false);
				break;
			case end_comm:
				sleep_unblock_mode(EM2);
				i2c_state_m->state = start_comm;
				add_scheduled_event(i2c_state_m->callback);
				i2c_state_m->busy = false;
				break;
			default:
				EFM_ASSERT(false);
				break;
				}
	}
	else{
		EFM_ASSERT(false);
		}
}

bool i2c_busy(){
	return i2c_sm.busy;
}

