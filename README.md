# Scheduino
Time Triggered Cooperative scheduler library for Arduino

Examples are included in the ./examples folder

This is an implementation of a Time Triggered Cooperative (TTC) scheduler, based on the designs by Dr Michael J. Pont (see http://www.safetty.net/training/beginners)

##Task Creation

Create your tasks as seperate void functions (see https://www.arduino.cc/en/Reference/FunctionDeclaration):

```
void task_function_name() {  
  /* The code in here will be repeated by the scheduler */  
}
```
Functions can have any name and can call any other functions you like. 

##Scheduler configuration

Configure the scheduler in ```setup()```:

```
void setup() {
	/* Tell the scheduler how many tasks there are: */
	Schedule.begin(n);

	/* Tell the scheduler which tasks are functions, and your desired timing (see note 1). 
	 * Also include your setup code here as normal */

	/* ['run-once' setup code for 1st task] */
	Schedule.addTask(1st_task_function_name, task_offset, task_period);

	/* ... */

	/* ['run-once' setup code for nth task] */
	Schedule.addTask(nth_task_function_name, task_offset, task_period);

	/* Start the scheduler with a tick length, t ms (see note 2)*/
	Schedule.startTicks(t);
}
```
###Note 1:
Control the task timing using:  
```task_offset```: The time in 'ticks' between start-up and the first execution of the task.  
```task_period```: The time in 'ticks' between executions of the task.  


These two properties allow tasks to be spaced out in the timeline to provide reliable timing.

###Note 2:
Remember that ```t``` determines how long ```task_offset``` and ```task_period``` are.  

###Example:

Consider two tasks, Task A and Task B configured as follows:
```
Schedule.begin(2);

Schedule.addTask(taskA, 0, 5);
Schedule.addTask(taskB, 1, 10);

Schedule.startTicks(10);
```
Timeline:

![Schedule timeline](https://chrisbarlow.files.wordpress.com/2016/03/capture.png)

Task A will run every 50 ms starting at tick 0. Task B will run every 100 ms, but has an offset of 1, so it will first run in tick 1. It doesn't matter that Task B takes longer than 1 tick, because there is enough space for it to complete before Task A is run again.

##Execution

Run the scheduler in ```loop()```:  
```
void loop() {
    Schedule.runTasks();
}
```
All other code should be in the task functions, don't put anything else in ```loop()```.

##ADVANCED: Timing Analysis

It is possible to check the timing of tasks using an oscilloscope and configuring an analysis pin for a task. To do this, include the desired pin as a fourth argument when you add the task to the schedule:
```Schedule.addTask(task_function_name, task_offset, task_period, pin_number);```  

With an analysis pin enabled, the scheduler will set this pin high just before the task is run, and set it low again when the task completes. (Note there are some overheads involved in setting and clearing the pin, so the timing shown on the oscilloscope wont be spot on, but it'll give you an idea of when tasks are colliding)

##Find out more

For more information, see:
http://chrisbarlow.wordpress.com
