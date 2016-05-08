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
 * [offset] and [period] should be chosen to suit the application.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <arduino.h>
#include <Tasks.h>
#include "maestro.h"


MaestroPlugin maestro;

/* Private member variables */
servoControlSteps_t MaestroPlugin::maestroControlStep;
uint16_t MaestroPlugin::stepCount;
uint16_t MaestroPlugin::servoSpeeds[NUM_SERVOS];
uint16_t MaestroPlugin::servoAccels[NUM_SERVOS];
uint16_t MaestroPlugin::sequenceStep;
uint16_t MaestroPlugin::servoNum;
int16_t *MaestroPlugin::servoSequence;
int16_t *MaestroPlugin::servoTuningValues;

/* Public member functions */
void MaestroPlugin::init(void){
	int i;
	Serial.write(0xA1);
	Serial.read();

	for(i = 0; i < NUM_SERVOS; i++){
		servoSpeeds[i] = 0;
		servoAccels[i] = 0;
	}
	sequenceStep = 0;
	servoNum = 0;
	maestroControlStep = SEQUENCE_FINISHED;
}

servoControlSteps_t MaestroPlugin::getUpdateStatus(void){
	return maestroControlStep;
}

void MaestroPlugin::startNewSequence(int16_t *sequence, uint16_t count){
	// if(maestroControlStep == SEQUENCE_FINISHED){
		servoSequence = sequence;
		stepCount = count;

		sequenceStep = 0;
		maestroControlStep = SENDING_SEQUENCE;
	// }
	// else{
	// 	/* TODO: sequence queueing? */
	// }
}

void MaestroPlugin::setSpeeds(uint16_t speeds[]){
	static bool speedSet = false;
	int i;

	if(speedSet == false){
		for(i = 0; i < NUM_SERVOS; i++){
			servoSpeeds[i] = 0xFFFF;
		}
		speedSet = true;
	}

	for(i = 0; i < NUM_SERVOS; i++){
		if(speeds[i] != servoSpeeds[i]){
			maestroCommandServo(i, MAESTRO_SET_SPEED, speeds[i]);
			servoSpeeds[i] = speeds[i];
		}
	}
}

void MaestroPlugin::setAccelerations(uint16_t accels[]){
	static bool accelSet = false;
	int i;

	if(accelSet == false){
		for(i = 0; i < NUM_SERVOS; i++){
			servoAccels[i] = 0xFFFF;
		}
		accelSet = true;
	}

	for(i = 0; i < NUM_SERVOS; i++){
		if(accels[i] != servoAccels[i]){
			maestroCommandServo(i, MAESTRO_SET_ACCEL, accels[i]);
			servoAccels[i] = accels[i];
		}
	}
}

void MaestroPlugin::setServoTuning(int16_t *tuningValues){
	servoTuningValues = tuningValues;
}



/* Private member functions */
uint16_t MaestroPlugin::tunedPosition(int16_t positionValue, int16_t tuningValue){
	int16_t tunedValue;

	if(positionValue == 0){
		tunedValue = 0;
	}
	else{
		tunedValue = (positionValue + tuningValue);
		tunedValue *= 4;	/* Set command values are in 1/4 microseconds */
		if(tunedValue <= 0){
			tunedValue = 1;
		}
		else if(tunedValue > MAESTRO_TWOBYTE_MAX){
			tunedValue = MAESTRO_TWOBYTE_MAX;
		}
	}

	return (uint16_t)tunedValue;
}

void MaestroPlugin::maestroCommandServo(uint8_t servo, uint8_t cmd, uint16_t value){
	Serial.write(cmd);
	Serial.write(servo);
	Serial.write(value & 0x7F);
	Serial.write((value >> 7) & 0x7F);
	Serial.read();
}

uint8_t MaestroPlugin::maestroGetState(void){
	uint8_t state = 0xFF;

	Serial.write(MAESTRO_GET_STATE);
	state = Serial.read();

	return state;
}

void Maestro_pluginUpdate(void){
	int sequencePosition = 0;
	uint8_t state;

	switch(maestro.maestroControlStep){
		default:
		case SEQUENCE_FINISHED:
			/* Do nothing, wait for new sequence */
			break;

		case SENDING_SEQUENCE:
			sequencePosition = maestro.servoNum + (maestro.sequenceStep*NUM_SERVOS);
			maestro.maestroCommandServo(maestro.servoNum, MAESTRO_SET_TARGET, maestro.tunedPosition(maestro.servoSequence[sequencePosition],maestro.servoTuningValues[maestro.servoNum]));

			if(++maestro.servoNum >= NUM_SERVOS){
				maestro.servoNum = 0;
				maestro.sequenceStep++;
				maestro.maestroControlStep = WAIT_FOR_STOP;
			}
			break;

		case WAIT_FOR_STOP:

#ifdef PRETEND_TO_BE_STOPPED
#warning "test mode won't wait for servos to stop"
			state = 0;
#else
			state = maestro.maestroGetState();
#endif

			if((state == 0x00) && (maestro.sequenceStep >= maestro.stepCount)){
				maestro.sequenceStep = 0;
				maestro.maestroControlStep = SEQUENCE_FINISHED;
			}
			else if(state == 0x00){
				maestro.maestroControlStep = SENDING_SEQUENCE;
			}

			break;
	}
}
