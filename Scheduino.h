/*
 * Copyright (c) 2016 by Chris Barlow <chrisbarlow.wordpress.com>
 * Time-Triggered Cooperative Scheduler for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SCHEDUINO_H_
#define _SCHEDUINO_H_

#include "Arduino.h"

typedef volatile void taskFn;
typedef volatile void (*task_function_t)(void);

class TTduino{
  public:
    /* Create a task list */
    void begin(uint16_t numTasks);
    /* Call in setup() Adds a task to the task list */
    void addTask(task_function_t function, uint32_t period, uint32_t offset);
    /* Start the timer interrupt (call at the end of setup() )*/
    void startTicks(uint16_t period);
    /* Call as the only method in loop(). Handles scheduling of the tasks */
    void runTasks(void);

    /* local ISR function (not accessible to public) */
    friend void __isrTick(void);

  private:
    struct tasks{
    	task_function_t task_function;	/* function pointer */
    	task_function_t task_initFunction;	/* function pointer */
    	uint32_t        task_period;	/* period in ticks */
    	uint32_t        task_delay;		/* initial offset in ticks */
    };
    tasks* _taskList;
    uint16_t _tasksUsed;
    uint16_t _numTasks;
    bool _schedLock;
    void sleepNow(void);
};

extern TTduino Schedule;

#endif /* _SCHEDUINO_H_ */
