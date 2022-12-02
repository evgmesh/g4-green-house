/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "heap_lock_monitor.h"
#include "retarget_uart.h"

#include "ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"


static volatile int counter;
static DigitalIoPin SW1(1,8, DigitalIoPin::input, true);
static DigitalIoPin SW2(0,5, DigitalIoPin::input, true);
static DigitalIoPin SW3(0,6, DigitalIoPin::input, true);

static QueueHandle_t queue;

#ifdef __cplusplus
extern "C" {
#endif
void PIN_INT0_IRQHandler(void) {
	int data = 0;
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	SW3.read() ? data = -1 : data = 1;
	xQueueSendToBackFromISR(queue, &data, &xHigherPriorityWoken);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
    portEND_SWITCHING_ISR(xHigherPriorityWoken);
}


#ifdef __cplusplus
}
#endif

void interrupt_init(void);


/* The following is required if runtime statistics are to be collected
 * Copy the code to the source file where other you initialize hardware */
extern "C" {

void vConfigureTimerForRunTimeStats( void ) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}
/* end runtime statictics collection */

static void idle_delay()
{
	vTaskDelay(1);
}

void task1(void *params)
{
	(void) params;

	retarget_init();

#if 0	/* HUMIDITY */
	ModbusMaster node3(241); // Create modbus object that connects to slave id 241 (HMP60)
	node3.begin(9600); // all nodes must operate at the same speed!
	node3.idle(idle_delay); // idle function is called while waiting for reply from slave
	ModbusRegister RH(&node3, 256, true);
#endif
	/* CO2
	 *
	 */
	ModbusMaster node3(240); // Create modbus object that connects to slave id 241 (HMP60)
	node3.begin(9600); // all nodes must operate at the same speed!
	node3.idle(idle_delay); // idle function is called while waiting for reply from slave
	ModbusRegister RH(&node3, 256, true);

	DigitalIoPin relay(0, 27, DigitalIoPin::output); // CO2 relay
	relay.write(0);

	DigitalIoPin sw_a2(1, 8, DigitalIoPin::pullup, true);
	DigitalIoPin sw_a3(0, 5, DigitalIoPin::pullup, true);
	DigitalIoPin sw_a4(0, 6, DigitalIoPin::pullup, true);
	DigitalIoPin sw_a5(0, 7, DigitalIoPin::pullup, true);

	DigitalIoPin *rs = new DigitalIoPin(0, 29, DigitalIoPin::output);
	DigitalIoPin *en = new DigitalIoPin(0, 9, DigitalIoPin::output);
	DigitalIoPin *d4 = new DigitalIoPin(0, 10, DigitalIoPin::output);
	DigitalIoPin *d5 = new DigitalIoPin(0, 16, DigitalIoPin::output);
	DigitalIoPin *d6 = new DigitalIoPin(1, 3, DigitalIoPin::output);
	DigitalIoPin *d7 = new DigitalIoPin(0, 0, DigitalIoPin::output);
	LiquidCrystal *lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
	// configure display geometry
	lcd->begin(16, 2);
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd->setCursor(0, 0);
	// Print a message to the LCD.
	lcd->print("MQTT_FreeRTOS");


	while(true) {
		float rh;
		char buffer[32];

		vTaskDelay(2000);

		rh = RH.read();
		snprintf(buffer, 32, "CO2=%5.1f%", rh);
		printf("%s\n",buffer);
		lcd->setCursor(0, 1);
		// Print a message to the LCD.
		lcd->print(buffer);

	}
}

void vButtonTask(void *pvParams) {

		queue = xQueueCreate(50, sizeof(int));
		vQueueAddToRegistry(queue, "ButtonsQueue");
		counter = 10;
		interrupt_init();

		/* Make DEBUGOUT work - include retarget_uart*/
		retarget_init();

		int buffer;
		while(true) {
			if(xQueueReceive(queue, &buffer, (TickType_t) 5000)){
				counter += buffer;
				DEBUGOUT("%d\r\n", counter);
				vTaskDelay(50);
			} else
				counter = 10;
		}
}

extern "C" {
  void vStartSimpleMQTTDemo( void ); // ugly - should be in a header
}

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

	heap_monitor_setup();

	// initialize RIT (= enable clocking etc.)
	//Chip_RIT_Init(LPC_RITIMER);
	// set the priority level of the interrupt
	// The level must be equal or lower than the maximum priority specified in FreeRTOS config
	// Note that in a Cortex-M3 a higher number indicates lower interrupt priority
	//NVIC_SetPriority( RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

	xTaskCreate(task1, "test",
			configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

//	xTaskCreate(vButtonTask, "Task1", configMINIMAL_STACK_SIZE + 128, NULL,
//				(tskIDLE_PRIORITY + 1), (TaskHandle_t *) NULL);

	vStartSimpleMQTTDemo();
	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

void interrupt_init(void) {
    /* Initialize PININT driver */
	Chip_PININT_Init(LPC_GPIO_PIN_INT);

	/* Enable PININT clock */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);

	/* Reset the PININT block */
	Chip_SYSCTL_PeriphReset(RESET_PININT);

	/* Configure interrupt channel for the GPIO pin in INMUX block */
	Chip_INMUX_PinIntSel(0, 0, 5);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt*/
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(0));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_SetPriority(PIN_INT0_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);
	NVIC_EnableIRQ(PIN_INT0_IRQn);
}
