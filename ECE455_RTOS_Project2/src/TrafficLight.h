/*
 * TrafficLight.h
 *
 *  Created on: Feb 23, 2019
 *      Author: Matthew
 */

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

//DEFINES
#define bool int
#define TRUE 1
#define FALSE 0

//INCLUDES
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"


//Traffic Light Variables
typedef enum trafficLightState
{
	Red		= 0b100,
	Yellow	= 0b010,
	Green	= 0b001,
} trafficLightState;

typedef volatile struct trafficLight
{
	trafficLightState currentState;
	trafficLightState nextState;
	int baseDelay;
	int lightDelay;
	bool init;

}trafficLight;

//Globals
trafficLight TrafficLight;
xSemaphoreHandle xMutexTrafficLight = NULL;

//Function Prototypes
void TrafficLightControlTask( void* pvParameters);
void TrafficLightInit(trafficLight* trafficLight);


#endif /* TRAFFICLIGHT_H_ */
