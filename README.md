# Arduino Tasks
## A Time-Triggered Scheduler for Arduino


This is an implementation of a time-triggered scheduler, based on the designs by Dr Michael J. Pont (see http://www.safetty.net/training/beginners)

## Installation

Copy the *Tasks* library folder into *Arduino/libraries/*

Examples are included in *Tasks/examples*

Include the library in your sketch:

**Sketch --> Include Library --> Tasks**

or

```cpp
#include <Tasks.h>
```

## Task Creation

Create your tasks as separate void functions (see https://www.arduino.cc/en/Reference/FunctionDeclaration):

```cpp
void task_1_function() {  
  /* [Repeated ('looped') code for Task 1] */  
}

/* ... */

void task_n_function() {  
  /* [Repeated ('looped') code for Task n] */  
}
```
Task functions can have any name and can call any other functions you like. 

### Freeing yourself from the Arduino IDE

For more advanced programmers who know their way around C, Tasks gives you a clean way of writing your code in any text editor you like. Simply add a 'Tasks' folder to the sketch folder and add a sub-folder for each task. Give each task a .cpp and .h file as you normally would. You can then use the Arduino IDE just for task configuration and compiling / uploading your project. The rest of the coding can be done in a more 'comfortable' editor (You can do this anyway, but Tasks gives you a clean separation of concerns).

![Clean coding](https://chrisbarlow.files.wordpress.com/2016/05/capture.png)

## Scheduler configuration

Configure the scheduler in ```setup()```:

```cpp
void setup() {
	/* Tell the scheduler how many tasks there are: */
	Schedule.begin(n);

	/* Tell the scheduler which functions are tasks, and your desired timing (see note 1). 
	 * Also include your setup code here as normal: */

	/* ['run-once' setup code for 1st task] */
	Schedule.addTask("Task 1 Name", task_1_function, task_1_offset, task_1_period);
	/* The name can be anything you like, but must be 20 characters or less */ 
	
	/* ... */

	/* ['run-once' setup code for nth task] */
	Schedule.addTask("Task n Name", task_n_function, task_n_offset, task_n_period);

	/* Start the scheduler with a tick length, t ms (see note 2):*/
	Schedule.startTicks(t);
}
```
#### Note 1:
Control the task timing using:  
```task_offset```: The time in 'ticks' between start-up and the first execution of the task.  
```task_period```: The time in 'ticks' between executions of the task.  


These two properties allow tasks to be spaced out in the timeline to provide reliable timing.

#### Note 2:
The tick length, ```t```, determines how long ```task_offset``` and ```task_period``` are.  

## Task Reports
You can check that a task has been added successfully by using ```Schedule.lastAddedTask()``` after you add a task.

Configure the serial port as normal:
```cpp
Serial.begin(9600);
```

Print the last task report to the serial port:
```cpp
Schedule.addTask("Test Task", task_A_function, 0, 2000);
Serial.print(Schedule.lastAddedTask());
```

If all is well, you will see this in the terminal:
```
---------------------------------------------
Added Task 0: "Test Task"
---------------------------------------------
Offset:		0 ms
Period:		2000 ms
Timing:		NORMAL
T Analysis:	disabled
---------------------------------------------
```

If there are no tasks in the schedule you'll see this:
```
*** No tasks in schedule ***
```

If there isn't enough space in the schedule, you'll see this:
```
*** We're going to need a bigger schedule ***
```
If you see this message, you need to increase the number in ```Schedule.begin``` to match the number of tasks you want.


## Execution

Run the scheduler in ```loop()```:  
```cpp
void loop() {
    Schedule.runTasks();
}
```
All other code should be in the task functions, don't put anything else in ```loop()```.

The scheduler will automatically space the tasks out according to the configured schedule.

## Examples:

### Basic schedule

Consider two tasks, Task A and Task B configured as follows:
```cpp
Schedule.begin(2);

Schedule.addTask("Task A", taskA, 0, 5);
Schedule.addTask("Task B", taskB, 1, 10);

Schedule.startTicks(10);
```
Timeline:

![Schedule timeline](https://chrisbarlow.files.wordpress.com/2016/03/capture.png)

Task A will run every 50 ms starting at tick 0. Task B will run every 100 ms, but has an offset of 1, so it will first run in tick 1. It doesn't matter that Task B takes longer than 1 tick, because there is enough space for it to complete before Task A is run again.

### Prioritising tasks

If you have multiple tasks that are expected to run in the same tick, it's useful to know that the tasks will run in the order that you add them to the schedule in ```setup()```:

Two tasks, Task C and Task D configured as follows:
```cpp
Schedule.begin(2);

Schedule.addTask("Task C", taskC, 0, 1);
Schedule.addTask("Task D", taskD, 0, 5);

Schedule.startTicks(10);
```
Task C is a very short task, which will run in every tick. Task D is configured to run every 5 ticks. Because Task C is added to the schedule first, it will run before Task D.

![Schedule timeline 2](https://chrisbarlow.files.wordpress.com/2016/03/capture2.png)

Now, suppose it is more important that Task D runs exactly every 50 ms, and the timing of Task C can be sacrificed. The priority can be changed in setup:

```cpp
Schedule.begin(2);

Schedule.addTask("Task D", taskD, 0, 5);
Schedule.addTask("Task C", taskC, 0, 1);

Schedule.startTicks(10);
```
![Schedule timeline 3](https://chrisbarlow.files.wordpress.com/2016/03/capture3.png)

It depends on the application to decide which task requires the most precise timing. Generally controlling (setting pins) or reading tasks need a higher priority than reporting tasks.

## ADVANCED

### Task Preemption

In the situation below, Task E takes longer than 2 ticks and is a lower priority than task C, which is required to run in every tick.

```cpp
Schedule.begin(2);

Schedule.addTask("Task C", taskC, 0, 1);
Schedule.addTask("Task E", taskE, 0, 10);

Schedule.startTicks(10);
```

Using standard (cooperative) scheduling, this will result in undesirable behaviour (priority inversion). Task C has to wait for Task E to complete, so even though it has a higher priority, it misses execution slots and runs late when Task E does complete.

![Priority inversion](https://chrisbarlow.files.wordpress.com/2016/04/capture4.png)

This can be overcome by allowing Task C to interrupt Task E when it needs to run. This is known as 'task preemption' and is acheived by setting the ```TIMING_FORCED``` option when adding the task:

```cpp
Schedule.begin(2);

Schedule.addTask("Task C", taskC, 0, 1, TIMING_FORCED);
Schedule.addTask("Task E", taskE, 0, 10);

Schedule.startTicks(10);
```

Now the tasks execute as required, with Task C maintaining its higher priority:

![Corrected timeline with preemption](https://chrisbarlow.files.wordpress.com/2016/04/capture5.png)

**NOTE: Tasks added with TIMING_FORCED must be shorter than 1 tick, otherwise ticks will be missed**

### Timing Analysis

It is possible to check the timing of tasks using an oscilloscope and configuring an analysis pin for a task. To do this, include the desired pin as a fourth argument when you add the task to the schedule:

```cpp
Schedule.addTask(task_function_name, task_offset, task_period, pin_number);
```  
or
```cpp
Schedule.addTask(task_function_name, task_offset, task_period, TIMING_FORCED, pin_number);
```  

With an analysis pin enabled, the scheduler will set this pin high just before the task is run, and set it low again when the task completes. (Note there are some overheads involved in setting and clearing the pin, so the timing shown on the oscilloscope wont be spot-on, but it'll give you an idea of when tasks are colliding)

## Find out more

For more information, see:
http://chrisbarlow.wordpress.com
