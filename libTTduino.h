// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _libTTduino_H_
#define _libTTduino_H_

typedef volatile void taskInitFn;
typedef volatile void taskUpdateFn;

#include "Arduino.h"

typedef volatile void (*task_function_t)(void);
volatile void no_init(void);

class TTduino{
  public:
    /* Create a task list */
    void begin(uint16_t numTasks);
    /* Call in setup() Adds a task to the task list */
    void addTask(task_function_t init, task_function_t update, uint32_t period, uint32_t offset);
    /* Initialise the tasks via their _init functions before starting the timer */
    void setupTasks(void);
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

#endif /* _libTTduino_H_ */
