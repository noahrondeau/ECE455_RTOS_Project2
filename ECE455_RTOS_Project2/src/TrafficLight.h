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


//Traffic Light Variables
typedef enum trafficLightState
{
	Red,
	Yellow,
	Green,
} trafficLightState;

typedef volatile struct trafficLight
{
	trafficLightState currentState;
	trafficLightState nextState;
	int baseDelay;
	int lightDelay;
	bool init;

}trafficLight;

void TrafficLightControlTask( void* pvParameters, trafficLight* trafficLight);
void TrafficLightInit(trafficLight* trafficLight);


#endif /* TRAFFICLIGHT_H_ */
