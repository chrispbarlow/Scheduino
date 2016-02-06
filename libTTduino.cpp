// Do not remove the include below
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "libTTduino.h"

static void tick_Start(uint16_t period);
static void sleepNow(void);

bool schedLock = false;


/* Task properties */
TTduino::scheduler(uint16_t numTasks, uint16_t tickLength){
	_tasksUsed = 0;
	_numTasks = numTasks;
	_taskList = new tasks[_numTasks];
	_ticklength = tickLength;
}

void TTduino::addTask(task_function_t init, task_function_t task, uint32_t period, uint32_t offset){
	if(_tasksUsed >= _numTasks){
		Serial.println("Too many tasks - increase NUM_TASKS in libTTduino.h");
	}
	else{
		_taskList[_tasksUsed].task_initFunction = init;
		_taskList[_tasksUsed].task_function = task;
		_taskList[_tasksUsed].task_period = period;
		_taskList[_tasksUsed].task_delay = offset;
	}
	_tasksUsed++;
}

/*
 * The familiar Arduino setup() function: runs once when you press reset.
 * x_Init() functions contain initialisation code for the related tasks.
 */
void TTduino::begin(){
	int i;

	wdt_disable();			/* Disable the watchdog timer */
	for(i = 0; i < _tasksUsed; i++){
		(*_taskList[i].task_initFunction)();
	}

	tick_Start(_ticklength);
}

/*
 * Start the timer interrupts
 */
void tick_Start(uint16_t period){
	/* initialize Timer1 */
	cli(); 			/* disable global interrupts */
	TCCR1A = 0; 	/* set entire TCCR1A register to 0 */
	TCCR1B = 0; 	/* same for TCCR1B */

	/* set compare match register to desired timer count: */
	OCR1A = (16 * period); /* TICK_PERIOD is in microseconds */
	/* turn on CTC mode: */
	TCCR1B |= (1 << WGM12);
	/* enable timer compare interrupt: */
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS10);
	sei(); /* enable global interrupts (start the timer)*/
}


/*
 * The ISR runs periodically every TICK_PERIOD
 */
ISR(TIMER1_COMPA_vect){
	sleep_disable();        /* disable sleep */
	power_all_enable();		/* restore all power */
	schedLock = false;		/* allow scheduler to run */

}

/* The loop function handles scheduling of the tasks */
void TTduino::runScheduledTasks(void){
	int i;
	sleepNow();															/* Go to sleep. Woken by ISR loop continues, then sleep repeats */

/******************** Nothing happens until interrupt tick *****************************************/
//	if (schedLock == false)												/*schedLock prevents scheduler from running on non-timer interrupt */
	{
		for(i = 0; i < _tasksUsed; i++)									/* For every task in schedule */
		{
			if(_taskList[i].task_delay == 0)								/* Task is ready when task_delay = 0 */
			{
				_taskList[i].task_delay = (Tasks[i].task_period - 1);		/* Reload task_delay */
				(*_taskList[i].task_function)();							/* Call task function */
			}
			else
			{
				_taskList[i].task_delay--;									/* task delay decremented until it reaches zero (time to run) */
			}
		}
	}
}


void sleepNow(){

	set_sleep_mode(SLEEP_MODE_IDLE);  									/* sleep mode is set here */

	sleep_enable();														/* enables the sleep bit in the mcucr register */
																		/* so sleep is possible. just a safety pin */
	power_adc_disable();												/* Power down some things to save power */
	power_spi_disable();
	power_timer0_disable();
	power_twi_disable();

	schedLock = true;													/* Prevent schedule from running on accidental wake-up */
	sleep_mode();            											/* here the device is actually put to sleep!! */
																		/* THE PROGRAM IS WOKEN BY TIMER1 ISR */
}
