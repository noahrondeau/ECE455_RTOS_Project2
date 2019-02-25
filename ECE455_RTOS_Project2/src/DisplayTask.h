/*
 * DisplayTask.h
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#ifndef DISPLAYTASK_H_
#define DISPLAYTASK_H_

#include "config.h"
#include "TrafficLight.h"

extern trafficLightState lightState;
extern uint32_t oncomingTrafficBitField;
extern uint32_t intersectionTrafficBitField;
extern uint32_t outgoingTrafficBitField;
extern SemaphoreHandle_t xBitFieldMutex;

void vDisplayTask( void* pvParameters );

#endif /* DISPLAYTASK_H_ */
