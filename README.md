# libTTduino
Time Triggered Cooperative scheduler library for Arduino

Examples are included in the ./examples folder

This is an implementation of a Time Triggered Cooperative (TTC) scheduler, based on the designs by Dr Michael J. Pont (see http://www.safetty.net/training/beginners)

Start a schedule with x tasks:

`TTduino Schedule(x);`


Create your tasks by adding writing two functions:

```
taskInitFn task_init(){  
/* Initialisation code goes here if you need any */  
}
    
taskUpdateFn task_update(){  
  /* The code in here will be repeated in the scheduler */  
}
```

(Functions can have any name you like, but it's good practice to use ```_init``` and ```_update``` to tell them apart.)

Add your tasks to the schedule in ```setup()```:

```Schedule.addTask(task_init, task_update, task_period, task_offset);```  
repeat for all of your tasks


If your task doesn't need any special initialisation code don't bother writing an empty function, you can use ```no_init```:  
```Schedule.addTask(no_init, task_update, task_period, task_offset);```

Control the task timing using:  
```task_period```: The time in 'ticks' between executions of the task.  
```task_offset```: The time in 'ticks' between start-up and the first execution of the task.  

These two properties allow tasks to be spaced out in the timeline to provide reliable timing.

Start the scheduler running with a tick length t (in microseconds):  
```Schedule.begin(t); ```  
This will run all of the _init functions and then start the timer.

Remember that ```t``` determines how long ```task_period``` and ```task_offset``` are.  
```t``` = 1000 and ```task_period``` = 100 gives a period of 100 milliseconds



Run the scheduler in ```loop()```:  
```Schedule.runTasks(); ```



For more information, see:
http://chrisbarlow.wordpress.com/2012/09/13/reliable-task-scheduling-with-arduino-avr/

and

http://chrisbarlow.wordpress.com/2012/12/28/further-task-scheduling-with-arduino-avr/
