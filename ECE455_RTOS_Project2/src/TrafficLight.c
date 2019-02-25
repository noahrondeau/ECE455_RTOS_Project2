/*
 * TrafficLight.c
 *
 *  Created on: Feb 23, 2019
 *      Author: Matthew
 */

/*****INCLUDES*******/

#include "TrafficLight.h"

/*****FUNCTIONS******/
void TrafficLightInit(TrafficLight_t* trafficLight)
{
	trafficLight->currentState = Green;
	trafficLight->nextState = Yellow;
	trafficLight->baseDelay = 1000; //1000ms -> 1s base delay
	trafficLight->lightDelay = trafficLight->baseDelay;
	trafficLight->init = true;
}

void TrafficLightControlTask(void* pvParameters)
{
	//Parameter passed in from traffic flow task
	int trafficLoad = (int) pvParameters;


	while(1)
	{

	    if( xLightMutex != NULL )
	    {
	        // See if we can obtain the semaphore.  If the semaphore is not available
	        // wait 10 ticks to see if it becomes free.
	        if( xSemaphoreTake( xLightMutex, ( portTickType ) 10 ) == pdTRUE )
	        {

	    		/*
	    		If first time then state should be in green
	    		otherwise update currentState from previous time task running
	    		*/
	    		if(trafficLight.init) trafficLight.init=false;
	    		else trafficLight.currentState = trafficLight.nextState;


	    		//sets task delays based on load and current state
	    		switch (trafficLight.currentState)
	    		{
	    		case Red:
	    			trafficLight.nextState = Green;

	    			//if heavy traffic have a short red (3s) other wise have a long red (5s)
	    			if(trafficLoad) trafficLight.lightDelay = 3*trafficLight.baseDelay;
	    			else trafficLight.lightDelay = 5*trafficLight.baseDelay;
	    			break;

	    		case Yellow:
	    			trafficLight.nextState = Red;
	    			trafficLight.lightDelay = trafficLight.baseDelay;
	    			break;

	    		case Green:
	    			trafficLight.nextState = Yellow;

	    			//if heavy traffic have a long green (8s) other wise have a short green (4s)
	    			if(trafficLoad) trafficLight.lightDelay = 8*trafficLight.baseDelay;
	    			else trafficLight.lightDelay = 4*trafficLight.baseDelay;
	    			break;

	    		default:
	    			break;
	    		}

	    		xSemaphoreGive( xLightMutex);

	    		vTaskDelay(trafficLight.lightDelay);
	        }
	        else
	        {
	            // We could not obtain the semaphore and can therefore not access
	            // the shared resource safely.
	        	vTaskDelay(trafficLight.baseDelay);
	        }
	    }

	}
}
