/*
 * TrafficLight.c
 *
 *  Created on: Feb 23, 2019
 *      Author: Matthew
 */

/*****INCLUDES*******/

#include "TrafficLight.h"
#include "Messenger_Pigeon.h"

extern TrafficLight_t trafficLight;
extern OneShot_Timer trafficLightTimer;
extern SemaphoreHandle_t xLightMutex;
extern EventGroupHandle_t xEvent;
extern Messenger_Pigeon  g___messenger_pigeon___FROM_task1_TO_task2___fp32___traffic_flow_rate___between_0_and_1;
extern Messenger_Pigeon  g___messenger_pigeon___FROM_task1_TO_task3___fp32___traffic_flow_rate___between_0_and_1;

/*****FUNCTIONS******/
void vTrafficLightInit(TrafficLight_t* trafficLight)
{
	trafficLight->currentState = Green;
	trafficLight->nextState = Yellow;
	trafficLight->baseDelay = TIME_PERIOD; //1000ms -> 1s base delay
	trafficLight->lightDelay = trafficLight->baseDelay;
	trafficLight->init = true;
	OneShot_Timer___Init(&trafficLightTimer, 4000,TrafficCallback);
}

void TrafficCallback(TimerHandle_t tlTimer){
	if( xSemaphoreTake( xLightMutex, ( portTickType )10 ) == pdTRUE )
	{
		//changes the light state
		switch (trafficLight.currentState)
		{
			case Red:
				trafficLight.currentState = Green;
				break;

			case Yellow:
				trafficLight.currentState = Red;
				break;

			case Green:
				trafficLight.currentState = Yellow;
				break;
		}

		xSemaphoreGive(xLightMutex);
	}
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
	float rxFlow = 0;

	while(1)
	{

		Messenger_Pigeon___Receive (&g___messenger_pigeon___FROM_task1_TO_task3___fp32___traffic_flow_rate___between_0_and_1,(void*) &rxFlow);


		// See if we can obtain the semaphore.  If the semaphore is not available wait 10 ticks to see if it becomes free.
		if( xSemaphoreTake( xLightMutex, ( portTickType )10 ) == pdTRUE )
		{
			//checks to see if a change has light state has changed
			if(trafficLight.currentState == trafficLight.nextState)
			{

				//sets task delays based on received load and current state
				switch (trafficLight.currentState)
				{
					case Red:
						//if heavy traffic have a short red (3s) other wise have a long red (5s)
						trafficLight.nextState = Green;
						trafficLight.lightDelay = 10*trafficLight.baseDelay - (int)(round(rxFlow*4.0*(float)(trafficLight.baseDelay)));
						break;

					case Yellow:

						trafficLight.nextState = Red;
						trafficLight.lightDelay = 4*(trafficLight.baseDelay);

						break;

					case Green:
						//if heavy traffic have a long green (8s) other wise have a short green (4s)
						trafficLight.nextState = Yellow;
						trafficLight.lightDelay = 10*trafficLight.baseDelay + (int)(rxFlow*4.0*(float)(trafficLight.baseDelay));
						break;

					default:
						//should never reach here
						printf("light state error, resetting to Green Light");
						trafficLight.lightDelay = trafficLight.baseDelay;
						trafficLight.init = true;
						trafficLight.currentState = Green;
						break;
				}

				//Start timer with calculated amount dependent on light and conditions
				OneShot_Timer___Change_Period_and_Restart(&trafficLightTimer,trafficLight.lightDelay);
			}

			xSemaphoreGive(xLightMutex);


		}
	}
}
