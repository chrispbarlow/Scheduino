/*
 * Copyright (c) 2016 by Chris Barlow <chrisbarlow.wordpress.com>
 * Time-Triggered Cooperative Scheduler for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "Scheduino.h"
#include "Arduino.h"

#define SET_TA(t) (*(t.analysis_pin_port) |= t.analysis_pin_bit)
#define CLR_TA(t) (*(t.analysis_pin_port) &= ~t.analysis_pin_bit)

TTduino Schedule;

/* Create a task list */
void TTduino::begin(uint16_t numTasks){
	_tasksUsed = 0;
	_numTasks = numTasks;
	_taskList = (tasks*)malloc(numTasks*sizeof(tasks));
	_schedLock = true;
}

void TTduino::addToTaskList(task_function_t function, uint32_t period, uint32_t offset){
	_taskList[_tasksUsed].task_function = function;
	_taskList[_tasksUsed].task_period = period;
	_taskList[_tasksUsed].task_delay = offset;
}

/* Call in setup() Adds a task to the task list */
void TTduino::addTask(task_function_t function, uint32_t period, uint32_t offset){
	if(_tasksUsed < _numTasks){
		addToTaskList(function,period,offset);
		_taskList[_tasksUsed].analysis_pin_bit = 0;
		_taskList[_tasksUsed].analysis_pin_port = portOutputRegister(0);
		_tasksUsed++;
	}
}

void TTduino::addTask(task_function_t function, uint32_t period, uint32_t offset, uint8_t analysisPin){
	uint8_t port;
	if(_tasksUsed < _numTasks){
		addToTaskList(function,period,offset);

		pinMode(analysisPin, OUTPUT);
		digitalWrite(analysisPin, LOW);
		_taskList[_tasksUsed].analysis_pin_bit = digitalPinToBitMask(analysisPin);
		port = digitalPinToPort(analysisPin);
		_taskList[_tasksUsed].analysis_pin_port = portOutputRegister(port);
		_tasksUsed++;
	}
}

/* Start the timer interrupt (call at the end of setup() )*/
void TTduino::startTicks(uint16_t period){
	/* initialize Timer1 */
	wdt_disable();			/* Disable the watchdog timer */
	cli(); 			/* disable global interrupts */
	TCCR1A = 0; 	/* set entire TCCR1A register to 0 */
	TCCR1B = 0; 	/* same for TCCR1B */

	/* set compare match register to desired timer count: */
	OCR1A = (16000 * period); /* period is in milliseconds */
	/* turn on CTC mode: */
	TCCR1B |= (1 << WGM12);
	/* enable timer compare interrupt: */
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS10);
	sei(); /* enable global interrupts (start the timer)*/
}

/* Call as the only method in loop(). Handles scheduling of the tasks */
void TTduino::runTasks(void){
	uint16_t i;
	/* Go to sleep. Woken by ISR loop continues, then sleep repeats */
	sleepNow();
/******************** Nothing happens until interrupt tick *****************************************/

	/*_schedLock prevents scheduler from running on non-timer interrupt */
	if (_schedLock == false){
		for(i = 0; i < _tasksUsed; i++){
			/* Task is ready when task_delay = 0 */
			if(_taskList[i].task_delay == 0){
				/* Reload task_delay */
				_taskList[i].task_delay = (_taskList[i].task_period - 1);
				SET_TA(_taskList[i]);
				/* Call task function */
				(*_taskList[i].task_function)();
				CLR_TA(_taskList[i]);
			}
		}
	}
}


void TTduino::sleepNow(){
	set_sleep_mode(SLEEP_MODE_IDLE);  	/* sleep mode is set here */
	sleep_enable();											/* enables the sleep bit in the mcucr register */
																			/* so sleep is possible. just a safety pin */
	power_adc_disable();								/* Power down some things to save power */
	power_spi_disable();
	power_timer0_disable();
	power_twi_disable();

	_schedLock = true;									/* Prevent schedule from running on accidental wake-up */
	sleep_mode();            						/* here the device is actually put to sleep!! */
/* THE PROGRAM IS WOKEN BY TIMER1 ISR */
}

/* The ISR runs periodically every tick */
void __isrTick(){
	uint16_t i;
	sleep_disable();        /* disable sleep */
	power_all_enable();			/* restore all power */
	for(i = 0; i < Schedule._tasksUsed; i++){
		/* task delay decremented until it reaches zero (time to run) */
		if(Schedule._taskList[i].task_delay > 0){
			Schedule._taskList[i].task_delay--;
		}
	}
	Schedule._schedLock = false;		/* allow scheduler to run */
}
ISR(TIMER1_COMPA_vect){
	__isrTick();
}

