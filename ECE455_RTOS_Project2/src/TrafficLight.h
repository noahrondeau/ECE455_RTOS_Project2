/*
 * TrafficLight.h
 *
 *  Created on: Feb 23, 2019
 *      Author: Matthew
 */

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

//INCLUDES
#include "config.h"
#include "OneShot_Timer.h"


//Traffic Light Variables
typedef enum TrafficLightState
{
	Red		= 0b100,
	Yellow	= 0b010,
	Green	= 0b001,
} TrafficLightState_t;

typedef volatile struct TrafficLight
{
	TrafficLightState_t currentState;
	TrafficLightState_t nextState;
	int baseDelay;
	int lightDelay;
	bool init;

}TrafficLight_t;

//Function Prototypes
void vTrafficLightControlTask( void* pvParameters);
void vTrafficLightInit(TrafficLight_t* trafficLight);
void TrafficCallback(TimerHandle_t tlTimer);


#endif /* TRAFFICLIGHT_H_ */
