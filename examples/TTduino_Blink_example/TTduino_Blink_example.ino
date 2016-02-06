/**************************************************
 * TTduino_example
 * 
 * Simple example using the TTduino library.
 * Replicates the behaviour of the standard Arduino 'Blink' example, while outputting the LED status (ON or OFF) to the Serial port
 * 
 */

#include <libTTduino.h>

/* Define a schedule with 3 tasks */
TTduino Schedule(3);

/* Tasks are added to the schedule here in the form addTask(init_function, update_fuction, task_period, task_offset) */
void setup() {
  /* The LED is turned on every 2000 'ticks' */
  Schedule.addTask(ledOn_init, ledOn_update, 2000, 0);
  /* The LED is turned off every 2000 'ticks', offset by 1000 'ticks' (note 'no_init' is used to indicate the absense of an initialisation function */
  Schedule.addTask(no_init, ledOff_update, 2000, 1000);
  /* The status is output every 100 'ticks', offset by 1 'tick' */
  Schedule.addTask(statusOut_init, statusOut_update, 100, 1);
  
  /* Starting the scheduler with a tick length of 1000 microseconds. All _init functions will be executed at this point */
  Schedule.begin(1000);
}

/* It's best not to do anything in loop() except runTasks() - doing anything else here may make affect timing */
void loop() {
  Schedule.runTasks();
}

/********** Task Functions **********/

/* This task will switch ON the LED on pin 13 */
taskInitFn ledOn_init(){
  pinMode(13, OUTPUT);
}

taskUpdateFn ledOn_update(){
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)  
}

/* This task will switch OFF the LED on pin 13
 * NOTE: There is no need for an INIT function since the LED will be initialised in ledOn_init() */
taskUpdateFn ledOff_update(){  
  digitalWrite(13, LOW);   // turn the LED off by making the voltage LOW
}

/* This task sends the status of the LED pin to the serial port */
taskInitFn statusOut_init(){
  Serial.begin(9600);
}

taskUpdateFn statusOut_update(){
  int pinVal = digitalRead(13);
  Serial.print("LED is: ");
  Serial.println(pinVal ? "ON" : "OFF");  
}
