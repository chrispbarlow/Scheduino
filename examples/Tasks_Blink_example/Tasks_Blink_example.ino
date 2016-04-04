/**************************************************
 * Scheduino_Blink_example
 *
 * Simple example using the Scheduino library.
 * Replicates the behaviour of the standard Arduino 'Blink' example, while outputting the LED status (ON or OFF) to the Serial port
 *
 */
#include <Tasks.h>


/* Tasks are added to the schedule here in the form addTask(task_function_name, task_period, task_offset) */
void setup() {
  String addedTasks;
  /* Configure the serial port to 9600 baud - used for reportAddedTask and the statusOut task */
  Serial.begin(9600);
  Serial.print("Tasks for Arduino - Scheduler example\n\n");
 
  /* Create a schedule with 3 tasks */
  Schedule.begin(3);

  /* The LED is turned on every 2000 'ticks' and off every 2000 'ticks', offset by 1000 'ticks'*/  
  /* Configure the LED pin as an output */
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  /* Add the LED tasks to the schedule */
  Schedule.addTask("LED on", ledOn, 0, 2000);
  Serial.print(Schedule.lastAddedTask());  
  Schedule.addTask("LED off", ledOff, 1000, 2000);
  Serial.print(Schedule.lastAddedTask());  

  /* The status is output every 100 'ticks', offset by 1 'tick' */
  Schedule.addTask("Status Report", statusOut, 1, 100);
  Serial.print(Schedule.lastAddedTask());  
  
  /* Starting the scheduler with a tick length of 1 millisecond */
  Schedule.startTicks(1);

  /* If there are too many tasks, only the first n tasks will run,
   *  where n is the number of tasks in Schedule.begin(n)
   */
  if(Schedule.checkTooManyTasks() == true){
    Serial.println("Too many tasks");
  }

  /* The maximum tick length is 4194 ms */
  if(Schedule.checkTicksTooLong() == true){
    Serial.println("Ticks too long");
  }
}

/* It's best not to do anything in loop() except runTasks() - doing anything else here will affect timing */
void loop() {
  Schedule.runTasks();
}

/********** Task Functions **********/

/* This task will switch ON the LED on pin 13 */
void ledOn(){
  digitalWrite(13, HIGH);   /* turn the LED on (HIGH is the voltage level) */
}

/* This task will switch OFF the LED on pin 13 */
void ledOff(){
  digitalWrite(13, LOW);   /* turn the LED off by making the voltage LOW */
}

/* This task sends the status of the LED pin to the serial port */
void statusOut(){
  int pinVal = digitalRead(13);
  Serial.print("LED is: ");
  Serial.println(pinVal == HIGH ? "ON" : "OFF");
}
