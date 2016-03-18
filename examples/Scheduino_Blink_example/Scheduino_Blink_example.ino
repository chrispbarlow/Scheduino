/**************************************************
 * Scheduino_Blink_example
 *
 * Simple example using the Scheduino library.
 * Replicates the behaviour of the standard Arduino 'Blink' example, while outputting the LED status (ON or OFF) to the Serial port
 *
 */
#include <Scheduino.h>


/* Tasks are added to the schedule here in the form addTask(task_function_name, task_period, task_offset) */
void setup() {
  /* Create a schedule with 3 tasks */
  Schedule.begin(3);

  /* The LED is turned on every 2000 'ticks' and off every 2000 'ticks', offset by 1000 'ticks'*/
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Schedule.addTask(ledOn, 2000, 0);
  Schedule.addTask(ledOff, 2000, 1000);

  /* The status is output every 100 'ticks', offset by 1 'tick' */
  Serial.begin(9600);
  Schedule.addTask(statusOut, 100, 1, 10);

  /* Starting the scheduler with a tick length of 1000 microseconds */
  Schedule.startTicks(1000);
}

/* It's best not to do anything in loop() except runTasks() - doing anything else here may make affect timing */
void loop() {
  Schedule.runTasks();
}

/********** Task Functions **********/

/* This task will switch ON the LED on pin 13 */
void ledOn(){
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
}

/* This task will switch OFF the LED on pin 13 */
void ledOff(){
  digitalWrite(13, LOW);   // turn the LED off by making the voltage LOW
}

/* This task sends the status of the LED pin to the serial port */
void statusOut(){
  int pinVal = digitalRead(13);
  Serial.print("LED is: ");
  Serial.println(pinVal ? "ON" : "OFF");
}
