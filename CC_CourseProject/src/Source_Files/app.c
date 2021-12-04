/**
 * @file app.c
 * @author Case Chrisbacher
 * @date March 14th, 2021
 * @brief Application Code Functions
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"
#include <stdio.h>


//***********************************************************************************
// defined files
//***********************************************************************************
//#define BLE_TEST_ENABLED
#define TDD_SI_ENABLED
//#define CIRC_BUFF_TEST



//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static char in[64];
static char in_t[64];
static char in_l[64];

//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * 	Setup the application peripheral.
 *
 *
 * @details
 *	Starts by enabling the clock tree, then configures the peripheral GPIO,
 *	sets up the structures to be used by the peripherals open driver functions,
 *	and begins counting.
 *
 * @note
 *	Called in main function.
 *
 ******************************************************************************/

void app_peripheral_setup(void){
	cmu_open();
	gpio_open();
	scheduler_open();
	sleep_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);
	si7021_i2c_open();
	veml6030_i2c_open();
	ble_open(BLE_TX_DONE_CB, BLE_RX_DONE_CB);
	add_scheduled_event(BOOT_UP_CB);
	sleep_block_mode(SYSTEM_BLOCK_EM);

}

/***************************************************************************//**
 * @brief
 *	Initializes LETIMER with struct values
 *
 * @details
 *	This function first creates a struct of type APP_LETIMER_PWM_TypeDef and then initializes
 *	all of the parameters inside said struct with the passed in values. The struct is then passed
 *	into the letimer_pwm_open function and counting is started with letimer_start
 *
 * @note
 * 	Called in the app_letimer_setup function.
 *
 *
 * @param[in] period
 *	Defines the period length of PWM in seconds, floating point variable.
 *
 * @param[in] act_period
 * 	Defines the active period of PWM in seconds, floating point variable.
 *
 * @param[in] out0_route
 * 	Defines the output of 0 to the gpio port/pin, routing register.
 *
 * @param[in] out1_route
 * 	Defines the output of 0 to the gpio port/pin, routing register.
 *
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements
	APP_LETIMER_PWM_TypeDef pwm_struct;
		pwm_struct.debugRun = false;
		pwm_struct.enable = false;
		pwm_struct.out_pin_route0 = out0_route;
		pwm_struct.out_pin_route1 = out1_route;
		pwm_struct.out_pin_0_en = false;
		pwm_struct.out_pin_1_en = false;
		pwm_struct.period = period;
		pwm_struct.active_period = act_period;

		pwm_struct.uf_irq_enable = true;
		pwm_struct.comp0_irq_enable = false;
		pwm_struct.comp1_irq_enable = false;
		pwm_struct.comp0_cb = LETIMER0_COMP0_CB;
		pwm_struct.comp1_cb = LETIMER0_COMP1_CB;
		pwm_struct.uf_cb = LETIMER0_UF_CB;


	letimer_pwm_open(LETIMER0, &pwm_struct);
	letimer_start(LETIMER0, true);

}
/***************************************************************************//**
 * @brief
 * 	Call back (event) handler of the LETIMER0 UF Event
 *
 *
 * @details
 *	Clear / remove the call backs due to the call back being processed or serviced. Clearing the event
 *	makes it available to be called the next time the event is triggered
 *
 * @note
 *	Will be the main source of CallBacks, but not the only place.
 *
 ******************************************************************************/

void scheduled_letimer0_uf_cb(void){
	while(i2c_busy(I2C1));
	si7021_read(SI7021_READ_CB);
	while(i2c_busy(I2C1));
	si7021_read(SI7021_READ_CB_T);
	while(i2c_busy(I2C1));
	veml6030_read(VEML6030_READ_CB);



	remove_scheduled_event(LETIMER0_UF_CB);
}

/***************************************************************************//**
 * @brief
 * 	Call back (event) handler of the LETIMER0 Comp0 Event
 *
 *
 * @details
 *	Clear / remove the call backs due to the call back being processed or serviced. Clearing the event
 *	makes it available to be called the next time the event is triggered
 *
 * @note
 *	Will be the main source of CallBacks, but not the only place.
 *
 ******************************************************************************/
void scheduled_letimer0_comp0_cb(void){
	remove_scheduled_event(LETIMER0_COMP0_CB);
	EFM_ASSERT(false);
}

/***************************************************************************//**
 * @brief
 * 	Call back (event) handler of the LETIMER0 Comp1 Event
 *
 *
 * @details
 *	Clear / remove the call backs due to the call back being processed or serviced. Clearing the event
 *	makes it available to be called the next time the event is triggered
 *
 * @note
 *	Will be the main source of CallBacks, but not the only place.
 *
 ******************************************************************************/
void scheduled_letimer0_comp1_cb(void){
	remove_scheduled_event(LETIMER0_COMP1_CB);
	EFM_ASSERT(false);
}

/***************************************************************************//**
 * @brief
 * 	Call back handler for Si7021
 *
 *
 * @details
 *	Function turns on LED1 if the humidity is at a high enough value. Event is then cleared, making it available to be called when triggered next.
 *
 * @note
 *
 ******************************************************************************/

void service_si7021_humidity_done(){

	float humidity = 0;
		humidity = si7021_rel_humidity();
		if(humidity >= 30.0){
			GPIO_PinOutSet(LED1_PORT, LED1_PIN);
		}
		else{
			GPIO_PinOutClear(LED1_PORT, LED1_PIN);
		}

		if(humidity - (float)((int)(humidity)) == 0){
			sprintf(in, "\nHumidity = %2.0f %%\n", humidity);
		}
		else{
		sprintf(in, "\nHumidity = %2.1f %%\n", humidity);
		}

		ble_write(in, BLE_TX_DONE_CB);
		remove_scheduled_event(SI7021_READ_CB);


}

void service_si7021_temp_done(){
	float temp = 0;
	temp = si7021_rel_temp();
	if(temp - (float)((int)(temp)) == 0){
		sprintf(in_t, "\nTemp = %2.0f F\n", (double) temp);
	}
	else{
		sprintf(in_t, "\nTemp = %2.1f F\n", (double) temp);
	}

	ble_write(in_t, BLE_TX_DONE_CB);
	remove_scheduled_event(SI7021_READ_CB_T);

}

void service_veml6030_light_done(){
	float light = veml6030_rel_light();
	if(light - (float)((int)(light)) == 0){
		sprintf(in_l, "\n%2.0f lux\n", light);
	}
	else{
		sprintf(in_l, "\n%2.1f lux\n", light);
	}
	ble_write(in_l , BLE_TX_DONE_CB);
	remove_scheduled_event(VEML6030_READ_CB);
}

/***************************************************************************//**
 * @brief
 * 	Call back handler for Booting up
 *
 *
 * @details
 *	Every time the device boots up, it starts the letimer and prints out Hello World.
 *
 * @note
 * 	The ble_write function sends the input to the bluetooth device. Also includes a test to make sure the
 * 	bluetooth is working/ naming the bluetooth device. Need to uncomment BLE_TEST_ENABLED definition above to use test
 *
 ******************************************************************************/

void scheduled_boot_up_cb(){
	remove_scheduled_event(BOOT_UP_CB);
	letimer_start(LETIMER0, true);   // letimer_start will inform the LETIMER0 peripheral to begin counting.
#ifdef BLE_TEST_ENABLED
	bool bletest;
	bletest = ble_test("SuperCoolBLE");

	EFM_ASSERT(bletest);
	timer_delay(2000);
#endif

#ifdef TDD_SI_ENABLED
	si7021_tdd_config();
#endif

#ifdef CIRC_BUFF_TEST
	circular_buff_test();
#endif

	ble_write("\nHello World\n", BLE_TX_DONE_CB);

#ifdef CIRC_BUFF_TEST
	ble_write("ADC LAB\n", BLE_TX_DONE_CB);
	ble_write("CASE CHRISBACHER\n", BLE_TX_DONE_CB);
#endif
}

/***************************************************************************//**
 * @brief
 * 	Call back handler for bluetooth device
 *
 *
 * @details
 *	Removes the bluetooth event to enable use next time it is called.
 *
 * @note
 *
 ******************************************************************************/
void scheduled_ble_tx_cb(){
	EFM_ASSERT(BLE_TX_DONE_CB & get_scheduled_events());
	remove_scheduled_event(BLE_TX_DONE_CB);
}


