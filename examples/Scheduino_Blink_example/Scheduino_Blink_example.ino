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
  
  /* Configure the LED pin as an output */
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  /* Add the LED tasks to the schedule */
  Schedule.addTask(ledOn, 0, 2000);
  Schedule.addTask(ledOff, 1000, 2000);

  /* The status is output every 100 'ticks', offset by 1 'tick' */
  
  /* Configure the serial port to 9600 baud */
  Serial.begin(9600);
  /* Add the status report task to the schedule */
  Schedule.addTask(statusOut, 1, 100);

  /* Starting the scheduler with a tick length of 1 millisecond */
  Schedule.startTicks(1);
}

/* It's best not to do anything in loop() except runTasks() - doing anything else here may make affect timing */
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
