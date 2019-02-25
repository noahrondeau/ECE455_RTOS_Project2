/*
 * TrafficLight.c
 *
 *  Created on: Feb 23, 2019
 *      Author: Matthew
 */

/*****INCLUDES*******/

#include "TrafficLight.h"

/*****FUNCTIONS******/
void vTrafficLightInit(TrafficLight_t* trafficLight)
{
	trafficLight->currentState = Green;
	trafficLight->nextState = Yellow;
	trafficLight->baseDelay = 1000; //1000ms -> 1s base delay
	trafficLight->lightDelay = trafficLight->baseDelay;
	trafficLight->init = true;
}

/****NOMINAL OPERATION******
 * Receives flow from generator.
 * On first system run light will be initially green,
 * each sequential run will transition the light.
 * Duration each light is on scaled from the base delay of the yellow light (1s)
 * Nominal length for green and red is 4s under no load
 * Light length is scaled using the float
 */
void vTrafficLightControlTask(void* pvParameters)
{
	//Parameter obtained from traffic flow task
	float trafficFlow;

	while(1)
	{

		if (xSemaphoreTake(xFlowMutex, (portTickType)10) == pdTRUE )
		{
			trafficFlow = flowRate;
			xSemaphoreGive(xFlowMutex);
		}

		// See if we can obtain the semaphore.  If the semaphore is not available wait 10 ticks to see if it becomes free.
		if( xSemaphoreTake( xLightMutex, ( portTickType ) 10 ) == pdTRUE )
		{

			/*
			If first time task is run the state should be in green
			otherwise update currentState from previous time task ran
			 */
			if(trafficLight.init) trafficLight.init=false;
				else trafficLight.currentState = trafficLight.nextState;


			//sets task delays based on received load and current state
			switch (trafficLight.currentState)
			{
			case Red:
				//if heavy traffic have a short red (3s) other wise have a long red (5s)
				trafficLight.nextState = Green;
				trafficLight.lightDelay = 4*trafficLight.baseDelay - (int)(trafficFlow*1000);
				break;

			case Yellow:

				trafficLight.nextState = Red;
				trafficLight.lightDelay = (trafficLight.baseDelay);

				break;

			case Green:
				//if heavy traffic have a long green (8s) other wise have a short green (4s)
				trafficLight.nextState = Yellow;
				trafficLight.lightDelay = 4*trafficLight.baseDelay + (int)(trafficFlow*4000);
				break;

			default:
				//should never reach here
				printf("light state error");
				break;
			}

			xSemaphoreGive(xLightMutex);

			//Delay by calculated amount dependent on light and conditions
			vTaskDelay(trafficLight.lightDelay);
		}
		else
		{
			// We could not obtain the semaphore therefore delay by base amount
			vTaskDelay(trafficLight.baseDelay);
		}
	}
}
