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

#include "Arduino.h"
#include "Tasks.h"

#define SET_TA(t) (*(t.analysis_pin_port) |= t.analysis_pin_bit)
#define CLR_TA(t) (*(t.analysis_pin_port) &= ~t.analysis_pin_bit)

TaskSchedule Schedule;

/********** Public methods **********/

/* Create a task list */
void TaskSchedule::begin(uint16_t numTasks){
	_tasksUsed = 0;
	_numTasks = numTasks;
	_taskList = (tasks*)malloc(numTasks*sizeof(tasks));
	_schedLock = true;
}

/* Call in setup() Adds a task to the task list */
void TaskSchedule::addTask(task_function_t function, uint32_t offset, uint32_t period){
	if(_tasksUsed < _numTasks){
		addToTaskList(function,offset,period,TIMING_NORMAL);
		disableTA();
		_tasksUsed++;
	}
	else{
		_errTooManyTasks = true;
	}
}

void TaskSchedule::addTask(task_function_t function, uint32_t offset, uint32_t period, timingType_t isPreemptive){
	if(_tasksUsed < _numTasks){
		addToTaskList(function,offset,period,isPreemptive);
		disableTA();
		_tasksUsed++;
	}
	else{
		_errTooManyTasks = true;
	}
}

void TaskSchedule::addTask(task_function_t function, uint32_t offset, uint32_t period, uint8_t analysisPin){
	uint8_t port;
	if(_tasksUsed < _numTasks){
		addToTaskList(function,offset,period,TIMING_NORMAL);
		enableTA(analysisPin);
		_tasksUsed++;
	}
	else{
		_errTooManyTasks = true;
	}
}

void TaskSchedule::addTask(task_function_t function, uint32_t offset, uint32_t period, timingType_t isPreemptive, uint8_t analysisPin){
	uint8_t port;
	if(_tasksUsed < _numTasks){
		addToTaskList(function,offset,period,isPreemptive);
		enableTA(analysisPin);
		_tasksUsed++;
	}
	else{
		_errTooManyTasks = true;
	}
}

/* Print a report of the last added task */
void TaskSchedule::reportAddedTask(){
	uint16_t lastTask;

	if(_errTooManyTasks){
		Serial.println("We're going to need a bigger schedule\n");
	}
	if(_tasksUsed > 0){
		lastTask = _tasksUsed - 1;
		Serial.print("Added task ");
		Serial.print(lastTask);
		Serial.println(":\n");

		Serial.print("Offset: ");
		Serial.println(_taskList[lastTask].task_delay);

		Serial.print("Period: ");
		Serial.println(_taskList[lastTask].task_period);

		Serial.println(_taskList[lastTask].preempt_flag ? "Timing set to TIMING_FORCED" : "Timing set to TIMING_NORMAL");
		Serial.println(_taskList[lastTask].analysis_pin_bit ? "Timing analysis enabled\n" : "Timing analysis disabled\n");
	}
	else{
		Serial.println("No tasks in schedule\n");
	}
}

/* Start the timer interrupt (call at the end of setup() )*/
void TaskSchedule::startTicks(uint16_t period){
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
void TaskSchedule::runTasks(void){
	uint16_t i;
	/* Go to sleep. Woken by ISR loop continues, then sleep repeats */
	sleepNow();
/******************** Nothing happens until interrupt tick *****************************************/

	/*_schedLock prevents scheduler from running on non-timer interrupt */
	if (_schedLock == false){
		for(i = 0; i < _tasksUsed; i++){
			if(_taskList[i].preempt_flag == TIMING_NORMAL){
				launchWhenReady(i);
			}
		}
	}
}



/********** Private methods **********/

void TaskSchedule::sleepNow(){
	set_sleep_mode(SLEEP_MODE_IDLE);  	/* sleep mode is set here */
	sleep_enable();						/* enables the sleep bit in the mcucr register */
										/* so sleep is possible. just a safety pin */
	power_adc_disable();				/* Power down some things to save power */
	power_spi_disable();
	power_timer0_disable();
	power_twi_disable();

	_schedLock = true;					/* Prevent schedule from running on accidental wake-up */
	sleep_mode();            			/* here the device is actually put to sleep!! */
/* THE PROGRAM IS WOKEN BY TIMER1 ISR */
}

void TaskSchedule::addToTaskList(task_function_t function, uint32_t offset, uint32_t period, timingType_t isPreemptive){
	_taskList[_tasksUsed].task_function = function;
	_taskList[_tasksUsed].task_period = period;
	_taskList[_tasksUsed].task_delay = offset;
	_taskList[_tasksUsed].preempt_flag = isPreemptive;
}

void TaskSchedule::launchWhenReady(uint16_t taskIndex){
	/* Task is ready when task_delay = 0 */
	if(_taskList[taskIndex].task_delay == 0){
		/* Reload task_delay */
		_taskList[taskIndex].task_delay = (_taskList[taskIndex].task_period - 1);
		SET_TA(_taskList[taskIndex]);
		/* Call task function */
		(*_taskList[taskIndex].task_function)();
		CLR_TA(_taskList[taskIndex]);
	}
}

void TaskSchedule::enableTA(uint8_t pin){
	uint8_t port;
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	_taskList[_tasksUsed].analysis_pin_bit = digitalPinToBitMask(pin);
	port = digitalPinToPort(pin);
	_taskList[_tasksUsed].analysis_pin_port = portOutputRegister(port);
}

void TaskSchedule::disableTA(void){
	_taskList[_tasksUsed].analysis_pin_bit = 0;
	_taskList[_tasksUsed].analysis_pin_port = portOutputRegister(0);
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

		if(Schedule._taskList[i].preempt_flag == TIMING_FORCED){
			Schedule.launchWhenReady(i);
		}
	}
	Schedule._schedLock = false;		/* allow scheduler to run */
}
ISR(TIMER1_COMPA_vect){
	__isrTick();
}
