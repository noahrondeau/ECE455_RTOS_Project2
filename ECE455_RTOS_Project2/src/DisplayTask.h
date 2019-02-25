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

extern TrafficLight_t trafficLight;
extern SemaphoreHandle_t xTrafficMutex;

void vDisplayTask( void* pvParameters );

#endif /* DISPLAYTASK_H_ */
