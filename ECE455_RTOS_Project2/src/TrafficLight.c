/*
 * TrafficLight.c
 *
 *  Created on: Feb 23, 2019
 *      Author: Matthew
 */
#include "TrafficLight.h"


void TrafficLightInit(trafficLight* trafficLight){
	trafficLight->currentState = Green;
	trafficLight->nextState = Yellow;
	trafficLight->baseDelay = 1000; //1000ms -> 1s base delay
	trafficLight->lightDelay = trafficLight->baseDelay;
}

void TrafficLightControlTask(void* pvParameters, trafficLight* trafficLight){
	int trafficLoad = (int) pvParameters;
	while(1){
		if(trafficLoad == 1 && (trafficLight->currentState) == Red ){ //Heavy Load, Less red light more green light
			trafficLight->lightDelay = 0.5*(trafficLight->baseDelay);
			trafficLight->nextState = Green;
		}
	}
}
