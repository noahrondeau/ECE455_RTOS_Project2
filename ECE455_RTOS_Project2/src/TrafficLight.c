/*
 * TrafficLight.c
 *
 *  Created on: Feb 23, 2019
 *      Author: Matthew
 */

/*****INCLUDES*******/

#include "TrafficLight.h"

/*****FUNCTIONS******/
void TrafficLightInit(trafficLight* trafficLight)
{
	trafficLight->currentState = Green;
	trafficLight->nextState = Yellow;
	trafficLight->baseDelay = 1000; //1000ms -> 1s base delay
	trafficLight->lightDelay = trafficLight->baseDelay;
	trafficLight->init = TRUE;

	xMutexTrafficLight = xSemaphoreCreateMutex();
}

void TrafficLightControlTask(void* pvParameters)
{
	//Parameter passed in from traffic flow task
	int trafficLoad = (int) pvParameters;


	while(1)
	{

	    if( xMutexTrafficLight != NULL )
	    {
	        // See if we can obtain the semaphore.  If the semaphore is not available
	        // wait 10 ticks to see if it becomes free.
	        if( xSemaphoreTake( xMutexTrafficLight, ( portTickType ) 10 ) == pdTRUE )
	        {

	    		/*
	    		If first time then state should be in green
	    		otherwise update currentState from previous time task running
	    		*/
	    		if(TrafficLight.init) TrafficLight.init=FALSE;
	    		else TrafficLight.currentState = TrafficLight.nextState;


	    		//sets task delays based on load and current state
	    		switch (TrafficLight.currentState)
	    		{
	    		case Red:
	    			TrafficLight.nextState = Green;

	    			//if heavy traffic have a short red (3s) other wise have a long red (5s)
	    			if(trafficLoad) TrafficLight.lightDelay = 3*TrafficLight.baseDelay;
	    			else TrafficLight.lightDelay = 5*TrafficLight.baseDelay;
	    			break;

	    		case Yellow:
	    			TrafficLight.nextState = Red;
	    			TrafficLight.lightDelay = TrafficLight.baseDelay;
	    			break;

	    		case Green:
	    			TrafficLight.nextState = Yellow;

	    			//if heavy traffic have a long green (8s) other wise have a short green (4s)
	    			if(trafficLoad) TrafficLight.lightDelay = 8*TrafficLight.baseDelay;
	    			else TrafficLight.lightDelay = 4*TrafficLight.baseDelay;
	    			break;

	    		default:
	    			break;
	    		}

	    		xSemaphoreGive( xMutexTrafficLight);

	    		vTaskDelay(TrafficLight.lightDelay);
	        }
	        else
	        {
	            // We could not obtain the semaphore and can therefore not access
	            // the shared resource safely.
	        	vTaskDelay(TrafficLight.baseDelay);
	        }
	    }

	}
}
