/*
 * TrafficLight.c
 *
 *  Created on: Feb 23, 2019
 *      Author: Matthew
 */

/*****INCLUDES*******/

#include "TrafficLight.h"


void TrafficLightInit(trafficLight* trafficLight)
{
	trafficLight->currentState = Green;
	trafficLight->nextState = Yellow;
	trafficLight->baseDelay = 1000; //1000ms -> 1s base delay
	trafficLight->lightDelay = trafficLight->baseDelay;
	trafficLight->init = TRUE;
}

void TrafficLightControlTask(void* pvParameters, trafficLight* trafficLight)
{
	//Parameter passed in from traffic flow task
	int trafficLoad = (int) pvParameters;

	while(1)
	{
		/*
		If first time then state should be in green
		otherwise update currentState from previous time task running
		*/
		if(trafficLight->init) trafficLight->init=FALSE;
		else trafficLight->currentState = trafficLight->nextState;


		//sets task delays based on load and current state
		switch (trafficLight->currentState)
		{
		case Red:
			trafficLight->nextState = Green;

			//if heavy traffic have a short red (3s) other wise have a long red (5s)
			if(trafficLoad) trafficLight->lightDelay = 3*trafficLight->baseDelay;
			else trafficLight->lightDelay = 5*trafficLight->baseDelay;
			break;

		case Yellow:
			trafficLight->nextState = Red;
			trafficLight->lightDelay = trafficLight->baseDelay;
			break;

		case Green:
			trafficLight->nextState = Yellow;

			//if heavy traffic have a long green (8s) other wise have a short green (4s)
			if(trafficLoad) trafficLight->lightDelay = 8*trafficLight->baseDelay;
			else trafficLight->lightDelay = 4*trafficLight->baseDelay;
			break;

		default:
			break;
		}

		vTaskDelay(trafficLight->lightDelay);
	}
}
