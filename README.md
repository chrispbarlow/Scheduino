# Scheduino
Time Triggered Cooperative scheduler library for Arduino

Examples are included in the ./examples folder

This is an implementation of a Time Triggered Cooperative (TTC) scheduler, based on the designs by Dr Michael J. Pont (see http://www.safetty.net/training/beginners)

Create your tasks as seperate void functions (see https://www.arduino.cc/en/Reference/FunctionDeclaration):

```
void task_function_name() {  
  /* The code in here will be repeated by the scheduler */  
}
```
Functions can have any name and can call any other functions you like. 


Configure the scheduler in ```setup()```:

```
void setup() {
	/* Tell the scheduler how many tasks there are: */
	Schedule.begin(n);

	/* Tell the scheduler which tasks are functions, and your desired timing (see note 1). Also include your setup code here as normal */
	/* ['run-once' setup code for 1st task] */
	Schedule.addTask(1st_task_function_name, task_period, task_offset);
	/* ... */
	/* ['run-once' setup code for nth task] */
	Schedule.addTask(nth_task_function_name, task_period, task_offset);

	/* Start the scheduler with a tick length, t (see note 2)*/
	Schedule.startTicks(t);
}
```
Note 1:
	Control the task timing using:  
	```task_period```: The time in 'ticks' between executions of the task.  
	```task_offset```: The time in 'ticks' between start-up and the first execution of the task.  

	These two properties allow tasks to be spaced out in the timeline to provide reliable timing.

Note 2:
	Remember that ```t``` determines how long ```task_period``` and ```task_offset``` are.  
	e.g. ```t``` = 1000 and ```task_period``` = 100 gives a period of 100,000 microseconds, or 100 milliseconds. ```task_offset``` = 1 will run the task 1000 microseconds after the start of the schedule.


Run the scheduler in ```loop()```:  
```
void loop() {
    Schedule.runTasks();
}
```
All other code should be in the task functions, don't put anything else in ```loop()```.

ADVANCED: Timing Analysis

It is possible to check the timing of tasks using an oscilloscope and configuring an analysis pin for a task. To do this, include the desired pin as a fourth argument when you add the task to the schedule:
```Schedule.addTask(task_function_name, task_period, task_offset, pin_number);```  

With an analysis pin enabled, the scheduler will set this pin high just before the task is run, and set it low again when the task completes. (Note there are some overheads involved in setting and clearing the pin, so the timing shown on the oscilloscope wont be spot on, but it'll give you an idea of when tasks are colliding)

For more information, see:
http://chrisbarlow.wordpress.com
