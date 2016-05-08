/*
 * Copyright (c) 2016 by Chris Barlow <chrisbarlow.wordpress.com>
 * Maestro plugin for arduino-tasks, a time-triggered scheduler.
 *
 * Supports the Pololu Mini-Maestro 12 servo controller
 * for other Maestro controllers, change the NUM_SERVOS property.
 *
 * To use this plugin with the Tasks library, use
 * Scheduler.enablePlugin(maestro.pluginTask, [offset], [period]);
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef maestro_H_
#define maestro_H_
#include <Tasks.h>

#define NUM_SERVOS					(12)

#define MAESTRO_SET_TARGET			(0x84)
#define MAESTRO_SET_MULTI_TARGET	(0x9F)
#define MAESTRO_SET_SPEED			(0x87)
#define MAESTRO_SET_ACCEL			(0x89)
#define MAESTRO_GET_STATE			(0x93)

/* 1 bit of every byte is lost because the msb is always left clear */
#define MAESTRO_TWOBYTE_MAX			(0x3FFF)

// #define PRETEND_TO_BE_STOPPED

typedef enum {SEQUENCE_FINISHED=0x0A, SENDING_SEQUENCE=0x0B, WAIT_FOR_STOP=0x0C} servoControlSteps_t;

void Maestro_pluginUpdate(void);

class MaestroPlugin{
public:
	friend void Maestro_pluginUpdate(void);
	MaestroPlugin() : pluginTask("Servo Controller", Maestro_pluginUpdate, TIMING_NORMAL){}
	TaskPlugin pluginTask;

	void init(void);
	servoControlSteps_t getUpdateStatus(void);
	void startNewSequence(int16_t *sequence, uint16_t count);
	void setSpeeds(uint16_t speeds[]);
	void setAccelerations(uint16_t accels[]);
	void setServoTuning(int16_t *tuningValues);

private:
	uint16_t tunedPosition(int16_t positionValue, int16_t tuningValue);
	void maestroCommandServo(uint8_t servo, uint8_t cmd, uint16_t value);
	uint8_t maestroGetState(void);


	static servoControlSteps_t maestroControlStep;
	static uint16_t stepCount;
	static uint16_t servoSpeeds[];
	static uint16_t servoAccels[];
	static uint16_t sequenceStep;
	static uint16_t servoNum;
	static int16_t *servoSequence;
	static int16_t *servoTuningValues;
};

extern MaestroPlugin maestro;

#endif /* maestro_H_ */
