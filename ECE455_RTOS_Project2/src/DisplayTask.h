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

// the tasks for calculating positions of cars and displaying
// this is run every 1 second
void vDisplayTask( void* pvParameters );

#endif /* DISPLAYTASK_H_ */
