/**
 * @file ble.c
 * @author
 * @date
 * @brief Contains all the functions to interface the application with the HM-18
 *   BLE module and the LEUART driver
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "ble.h"
#include <string.h>

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
static CIRC_TEST_STRUCT test_struct;
static BLE_CIRCULAR_BUF ble_cbuf;

/***************************************************************************//**
 * @brief BLE module
 * @details
 *  This module contains all the functions to interface the application layer
 *  with the HM-18 Bluetooth module.  The application does not have the
 *  responsibility of knowing the physical resources required, how to
 *  configure, or interface to the Bluetooth resource including the LEUART
 *  driver that communicates with the HM-18 BLE module.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************
static void ble_circ_init(void);
static void ble_circ_push(char*string);
static uint8_t ble_circ_space(void);
static void update_circ_wrtindex(BLE_CIRCULAR_BUF*index_struct, uint32_t update_by);
static void update_circ_readindex(BLE_CIRCULAR_BUF*index_struct, uint32_t update_by);


/***************************************************************************//**
 * @brief
 * Opens Bluetooth module
 *
 * @details
 * 	Initializes a local struct with the values inputed into the ble.h file and then passes the struct into the leuart_open function
 *
 * @param[in] tx_event
 * 	Transmission Event.
 *
 * @param[in] out1_route
 * 	Recieving event.
 *
 ******************************************************************************/

void ble_open(uint32_t tx_event, uint32_t rx_event){

	LEUART_OPEN_STRUCT ble_values;
	ble_values.baudrate = HM18_BAUDRATE;
	ble_values.databits = HM18_DATABITS;
	ble_values.enable = HM18_ENABLE;
	ble_values.parity = HM18_PARITY;
	ble_values.stopbits = HM18_STOPBITS;
	ble_values.refFreq = HM18_REFFREQ;
	ble_values.tx_loc = LEUART_ROUTELOC0_TXLOC_LOC18;
	ble_values.rx_loc = LEUART_ROUTELOC0_RXLOC_LOC18;
	ble_values.tx_pin_en = true;
	ble_values.rx_pin_en = true;

	leuart_open(HM18_LEUART0, &ble_values);
	ble_circ_init();

}


/***************************************************************************//**
 * @brief
 * 	Begins process of sending text over bluetooth
 *
 * @details
 * 	Calls the leuart_start function to begin sending text
 *
 * @param[in] string
 * 	String to be sent over bluetooth.
 *
 * @param[in] tx_call_back_done
 * 	Event that is triggered when the transmission is finished.
 ******************************************************************************/

void ble_write(char* string, uint32_t tx_call_back_done){

	leuart_start(HM18_LEUART0, string, tx_call_back_done);
}

/***************************************************************************//**
 * @brief
 *   BLE Test performs two functions.  First, it is a Test Driven Development
 *   routine to verify that the LEUART is correctly configured to communicate
 *   with the BLE HM-18 module.  Second, the input argument passed to this
 *   function will be written into the BLE module and become the new name
 *   advertised by the module while it is looking to pair.
 *
 * @details
 * 	 This global function will use polling functions provided by the LEUART
 * 	 driver for both transmit and receive to validate communications with
 * 	 the HM-18 BLE module.  For the assignment, the communication with the
 * 	 BLE module must use low energy design principles of being an interrupt
 * 	 driven state machine.
 *
 * @note
 *   For this test to run to completion, the phone most not be paired with
 *   the BLE module.  In addition for the name to be stored into the module
 *   a breakpoint must be placed at the end of the test routine and stopped
 *   at this breakpoint while in the debugger for a minimum of 5 seconds.
 *
 * @param[in] *mod_name
 *   The name that will be written to the HM-18 BLE module to identify it
 *   while it is advertising over Bluetooth Low Energy.
 *
 * @return
 *   Returns bool true if successfully passed through the tests in this
 *   function.
 ******************************************************************************/

bool ble_test(char *mod_name){
	uint32_t	str_len;

	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	// This test will limit the test to the proper setup of the LEUART
	// peripheral, routing of the signals to the proper pins, pin
	// configuration, and transmit/reception verification.  The test
	// will communicate with the BLE module using polling routines
	// instead of interrupts.

	// How is polling different than using interrupts?
	// ANSWER: Polling is when the device constantly asks if an event is ready
	// instead of waiting for an interrupt to occur.

	// How does interrupts benefit the system for low energy operation?
	// ANSWER: Interrupts allow the CPU to go to sleep when not executing instructions
	// The CPU only wakes when it is time for an event.

	// How does interrupts benefit the system that has multiple tasks?
	// ANSWER: It allows the system to do multiple tasks, seemingly at parallel.
	// This is far superior to waiting for one task to complete before beginning
	// the next task.


	// First, you will need to review the DSD HM10 datasheet to determine
	// what the default strings to write data to the BLE module and the
	// expected return statement from the BLE module to test / verify the
	// correct response

	// The test_str is used to tell the BLE module to end a Bluetooth connection
	// such as with your phone.  The ok_str is the result sent from the BLE module
	// to the micro-controller if there was not active BLE connection at the time
	// the break command was sent to the BLE module.
	// Replace the test_str "" with the command to break or end a BLE connection
	// Replace the ok_str "" with the result that will be returned from the BLE
	//   module if there was no BLE connection
	char		test_str[80] = "AT";
	char		ok_str[80] = "OK";


	// output_str will be the string that will program a name to the BLE module.
	// From the DSD HM10 datasheet, what is the command to program a name into
	// the BLE module?
	// The  output_str will be a string concatenation of the DSD HM10 command
	// and the input argument sent to the ble_test() function
	// Replace the output_str "" with the command to change the program name
	// Replace the result_str "" with the first part of the expected result
	//  the backend of the expected response will be concatenated with the
	//  input argument
	char		output_str[80] = "AT+NAME";
	char		result_str[80] = "OK+SET:";


	// To program the name into your module, you must reset the module after you
	// have sent the command to update the modules name.  What is the DSD HM10
	// name to reset the module?
	// Replace the reset_str "" with the command to reset the module
	// Replace the reset_result_str "" with the expected BLE module response to
	//  to the reset command
	char		reset_str[80] = "AT+RESET";
	char		reset_result_str[80] = "OK+RESET";
	char		return_str[80];

	bool		success;
	bool		rx_disabled, rx_en, tx_en;
	uint32_t	status;

	// These are the routines that will build up the entire command and response
	// of programming the name into the BLE module.  Concatenating the command or
	// response with the input argument name
	strcat(output_str, mod_name);
	strcat(result_str, mod_name);

	// The test routine must not alter the function of the configuration of the
	// LEUART driver, but requires certain functionality to insure the logical test
	// of writing and reading to the DSD HM10 module.  The following c-lines of code
	// save the current state of the LEUART driver that will be used later to
	// re-instate the LEUART configuration

	status = leuart_status(HM18_LEUART0);
	if (status & LEUART_STATUS_RXBLOCK) {
		rx_disabled = true;
		// Enabling, unblocking, the receiving of data from the LEUART RX port
		leuart_cmd_write(HM18_LEUART0, LEUART_CMD_RXBLOCKDIS);
	}
	else rx_disabled = false;
	if (status & LEUART_STATUS_RXENS) {
		rx_en = true;
	} else {
		rx_en = false;
		// Enabling the receiving of data from the RX port
		leuart_cmd_write(HM18_LEUART0, LEUART_CMD_RXEN);
		while (!(leuart_status(HM18_LEUART0) & LEUART_STATUS_RXENS));
	}

	if (status & LEUART_STATUS_TXENS){
		tx_en = true;
	} else {
		// Enabling the transmission of data to the TX port
		leuart_cmd_write(HM18_LEUART0, LEUART_CMD_TXEN);
		while (!(leuart_status(HM18_LEUART0) & LEUART_STATUS_TXENS));
		tx_en = false;
	}
//	leuart_cmd_write(HM18_LEUART0, (LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX));

	// This sequence of instructions is sending the break ble connection
	// to the DSD HM10 module.
	// Why is this command required if you want to change the name of the
	// DSD HM10 module?
	// ANSWER: You can't change the name of the device without first breaking the connection.

	str_len = strlen(test_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM18_LEUART0, test_str[i]);
	}

	// What will the ble module response back to this command if there is
	// a current ble connection?
	// ANSWER: OK+LOST

	str_len = strlen(ok_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM18_LEUART0);
		if (ok_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// This sequence of code will be writing or programming the name of
	// the module to the DSD HM10
	str_len = strlen(output_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM18_LEUART0, output_str[i]);
	}

	// Here will be the check on the response back from the DSD HM10 on the
	// programming of its name
	str_len = strlen(result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM18_LEUART0);
		if (result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// It is now time to send the command to RESET the DSD HM10 module
	str_len = strlen(reset_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM18_LEUART0, reset_str[i]);
	}

	// After sending the command to RESET, the DSD HM10 will send a response
	// back to the micro-controller
	str_len = strlen(reset_result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM18_LEUART0);
		if (reset_result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// After the test and programming have been completed, the original
	// state of the LEUART must be restored
	if (!rx_en) leuart_cmd_write(HM18_LEUART0, LEUART_CMD_RXDIS);
	if (rx_disabled) leuart_cmd_write(HM18_LEUART0, LEUART_CMD_RXBLOCKEN);
	if (!tx_en) leuart_cmd_write(HM18_LEUART0, LEUART_CMD_TXDIS);
	leuart_if_reset(HM18_LEUART0);

	success = true;


	CORE_EXIT_CRITICAL();
	return success;
}

void ble_circ_init(void){
	ble_cbuf.size = CSIZE;
	ble_cbuf.size_mask = CSIZE -1;
	ble_cbuf.read_ptr = 0;
	ble_cbuf.write_ptr = 0;
}

void ble_circ_push(char*string){
	if(strlen(string) + 1 <= ble_circ_space()){
		ble_cbuf.cbuf[ble_cbuf.write_ptr] = strlen(string) + 1;
		for(int i = 0; i < strlen(string); i++){
			ble_cbuf.cbuf[ble_cbuf.write_ptr] = string[i];
		}
		update_circ_wrtindex(&ble_cbuf, strlen(string));
	}
	else{
		EFM_ASSERT(false);
	}
}

bool ble_circ_pop(bool test){					//WORK ON ME
	if(leuart_tx_busy(HM18_LEUART0)){
		return true;
	}
	if(ble_cbuf.read_ptr == ble_cbuf.write_ptr){
		return true;
	}
	int length = ble_cbuf.cbuf[ble_cbuf.read_ptr];
	char print_str[length - 1];
	for(int i = 0; i < length - 1; i++){
		print_str[i] = ble_cbuf.cbuf[(ble_cbuf.read_ptr + 1 + i) & ble_cbuf.size_mask];
	}
	update_circ_readindex(&ble_cbuf, length);
	if(test){
		for(int i = 0; i < length; i++){
			test_struct.result_str[i] = print_str[i];
		}
	}
	else{
		leuart_start(HM18_LEUART0, print_str, 0b000);				//questionable call back input
	}
	return false;

}

uint8_t ble_circ_space(void){
	uint8_t openspace = ble_cbuf.size - ((ble_cbuf.write_ptr - ble_cbuf.read_ptr) & ble_cbuf.size_mask);
	return openspace;
}

void update_circ_wrtindex(BLE_CIRCULAR_BUF*index_struct, uint32_t update_by){
	index_struct->write_ptr = (index_struct->write_ptr + update_by) & (index_struct->size_mask);
}

void update_circ_readindex(BLE_CIRCULAR_BUF*index_struct, uint32_t update_by){
	index_struct->read_ptr = (index_struct->read_ptr + update_by) & (index_struct->size_mask);
}


/***************************************************************************//**
 * @brief
 *   Circular Buff Test is a Test Driven Development function to validate
 *   that the circular buffer implementation
 *
 * @details
 * 	 This Test Driven Development test has tests integrated into the function
 * 	 to validate that the routines can successfully identify whether there
 * 	 is space available in the circular buffer, the write and index pointers
 * 	 wrap around, and that one or more packets can be pushed and popped from
 * 	 the circular buffer.
 *
 * @note
 *   If anyone of these test will fail, an EFM_ASSERT will occur.  If the
 *   DEBUG_EFM=1 symbol is defined for this project, exiting this function
 *   confirms that the push, pop, and the associated utility functions are
 *   working.
 *
 * @par
 *   There is a test escape that is not possible to test through this
 *   function that will need to be verified by writing several ble_write()s
 *   back to back and verified by checking that these ble_write()s were
 *   successfully transmitted to the phone app.
 *
 ******************************************************************************/

void circular_buff_test(void){
	 bool buff_empty;
	 int test1_len = 50;
	 int test2_len = 25;
	 int test3_len = 5;

	 // Why this 0 initialize of read and write pointer?
	 // Student Response: When we start the buffer is empty.
	 //	This means that the read and write pointers are initialized to
	 // the zero position because it is the beginning and the end of the string.

	 ble_cbuf.read_ptr = 0;
	 ble_cbuf.write_ptr = 0;

	 // Why do none of these test strings contain a 0?
	 // Student Response: The zero can be seen as a NULL which could be skipped
	 // during the process

	 for (int i = 0;i < test1_len; i++){
		 test_struct.test_str[0][i] = i+1;
	 }
	 test_struct.test_str[0][test1_len] = 0;

	 for (int i = 0;i < test2_len; i++){
		 test_struct.test_str[1][i] = i + 20;
	 }
	 test_struct.test_str[1][test2_len] = 0;

	 for (int i = 0;i < test3_len; i++){
		 test_struct.test_str[2][i] = i +  35;
	 }
	 test_struct.test_str[2][test3_len] = 0;

	 // What is this test validating?
	 // Student response: That we are initializing the size
	 // of the circular buffer correctly

	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // Why is there only one push to the circular buffer at this stage of the test
	 // Student Response: To test if a single push works before we move on to
	 // pushing multiple. Need to test base conditions.

	 ble_circ_push(&test_struct.test_str[0][0]);

	 // What is this test validating?
	 // Student response: That the single push worked, the space is reduced by 1

	 EFM_ASSERT(ble_circ_space() == (CSIZE - test1_len - 1));

	 // Why is the expected buff_empty test = false?
	 // Student Response: Because we popped the single element off of the buffer

	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test1_len; i++){
		 EFM_ASSERT(test_struct.test_str[0][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: This is making sure it is back to the original length
	 // after pushing and popping it off the buffer

	 EFM_ASSERT(strlen(test_struct.result_str) == test1_len);

	 // What is this test validating?
	 // Student response: This test is validating that the space is completely
	 // emty after pushing and popping off the buffer

	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // What does this next push on the circular buffer test?
	 // Student Response: This pushes the second string onto the buffer

	 ble_circ_push(&test_struct.test_str[1][0]);


	 EFM_ASSERT(ble_circ_space() == (CSIZE - test2_len - 1));

	 // What does this next push on the circular buffer test?
	 // Student Response: It makes sure the buffer can handle more
	 // than one string on it.

	 ble_circ_push(&test_struct.test_str[2][0]);


	 EFM_ASSERT(ble_circ_space() == (CSIZE - test2_len - 1 - test3_len - 1));

	 // What does this next push on the circular buffer test?
	 // Student Response: It tests that we don't allow a string that is larger
	 // than the available size to be put into the buffer


	 EFM_ASSERT(abs(ble_cbuf.write_ptr - ble_cbuf.read_ptr) < CSIZE);

	 // Why is the expected buff_empty test = false?
	 // Student Response: There are multiple test strings on the buffer
	 // so the buffer would not be empty, hence the false.

	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test2_len; i++){
		 EFM_ASSERT(test_struct.test_str[1][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: The popped string is the length of the
	 // expected string to be popped from the buffer.


	 EFM_ASSERT(strlen(test_struct.result_str) == test2_len);

	 EFM_ASSERT(ble_circ_space() == (CSIZE - test3_len - 1));

	 // Why is the expected buff_empty test = false?
	 // Student Response: There is still one string left on the buffer
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test3_len; i++){
		 EFM_ASSERT(test_struct.test_str[2][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: The popped string is the same length as the
	 // expected string to be popped

	 EFM_ASSERT(strlen(test_struct.result_str) == test3_len);

	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // Using these three writes and pops to the circular buffer, what other test
	 // could we develop to better test out the circular buffer?
	 // Student Response: We could test inputting null characters, spaces,
	 // and also testing border conditions. We want to make sure that the test
	 // works for ALL situations.

	 // Why is the expected buff_empty test = true?
	 // Student Response: All of the strings have been popped from the buffer.
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == true);
	 ble_write("\nPassed Circular Buffer Test\n", BLE_TX_DONE_CB);

 }

