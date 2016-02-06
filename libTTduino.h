// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _libTTduino_H_
#define _libTTduino_H_

#define TASK_INIT_FUNCTION volatile void
#define TASK_UPDATE_FUNCTION volatile void

#include "Arduino.h"

typedef volatile void (*task_function_t)(void);
volatile void no_init(void);

class TTduino{
  public:
    TTduino(uint16_t numTasks);
    void addTask(task_function_t init, task_function_t update, uint32_t period, uint32_t offset);
    void begin(uint16_t ticklength);
    void runTasks(void);
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
};

#endif /* _libTTduino_H_ */
