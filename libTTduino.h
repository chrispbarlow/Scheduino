// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _libTTduino_H_
#define _libTTduino_H_
#include "Arduino.h"

#define NUM_TASKS 	(10)		/* Total number of tasks */

class TTduino{
  public:
    scheduler(uint16_t numTasks);
    void addTask(task_function_t init, task_function_t task, uint32_t period, uint32_t offset);
    void runInit();
    void runScheduledTasks(void);
  private:
    typedef volatile void (*task_function_t)(void);
    struct tasks{
    	task_function_t task_function;	/* function pointer */
    	task_function_t task_initFunction;	/* function pointer */
    	uint32_t        task_period;	/* period in ticks */
    	uint32_t        task_delay;		/* initial offset in ticks */
    };
    tasks* _taskList;
    uint16_t _tasksUsed;
    uint16_t _numTasks;
    uint16_t _ticklength;
};

/*
 * Function pointer for task array
 * This links the Task list with the functions from the includes
 * */


// Prototypes


//Do not add code below this line
#endif /* _libTTduino_H_ */
