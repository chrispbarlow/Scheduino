# Scheduino
Time Triggered Cooperative scheduler library for Arduino

Examples are included in the ./examples folder

This is an implementation of a Time Triggered Cooperative (TTC) scheduler, based on the designs by Dr Michael J. Pont (see http://www.safetty.net/training/beginners)

Start a schedule with x tasks:

`Schedule.begin(x);`


Create your tasks by adding a function:

```
taskFn task_function_name(){  
  /* The code in here will be repeated in the scheduler */  
}
```

(Functions can have any name you like)

Add your tasks to the schedule in ```setup()```:
```Schedule.addTask(task_function_name, task_period, task_offset);```  
Repeat for all of your tasks.

Also include any 'run-once' code that you would usually put in ```setup()```.

Control the task timing using:  
```task_period```: The time in 'ticks' between executions of the task.  
```task_offset```: The time in 'ticks' between start-up and the first execution of the task.  

These two properties allow tasks to be spaced out in the timeline to provide reliable timing.

Right at the end of ```setup()```, start the timer running with a tick length, ```t``` (in microseconds):  
```Schedule.startTicks(t); ```  


Remember that ```t``` determines how long ```task_period``` and ```task_offset``` are.  
e.g. ```t``` = 1000 and ```task_period``` = 100 gives a period of 100,000 microseconds, or 100 milliseconds. ```task_offset``` = 1 will run the task 1000 microseconds after the start of the schedule.



Run the scheduler in ```loop()```:  
```Schedule.runTasks(); ```



For more information, see:
http://chrisbarlow.wordpress.com/2012/09/13/reliable-task-scheduling-with-arduino-avr/

and

http://chrisbarlow.wordpress.com/2012/12/28/further-task-scheduling-with-arduino-avr/
