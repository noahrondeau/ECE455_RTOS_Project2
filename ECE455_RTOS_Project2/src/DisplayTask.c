/*
 * DisplayTask.c
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#include "DisplayTask.h"
#include "ShiftReg.h"

extern TrafficLight_t trafficLight;
extern SemaphoreHandle_t xLightMutex;
extern EventGroupHandle_t xEvent;

static void ShiftOncomingWhenStopped(uint32_t* oncoming, bool carPresent)
{
	uint32_t searchMask = (uint32_t)(0b10000000);
	uint32_t shiftMask	= (uint32_t)(0b11111111);
	// start from bit 7, and look for most significant 0,
	// and move everything else up
	for ( int i = 0; i < 8; i++)
	{
		if ( ((*oncoming) & (searchMask >> i)) == 0 )
		{
			uint32_t topBits = (*oncoming) & (shiftMask << (8-i));
			uint32_t bottomBits = ((*oncoming) & (shiftMask >> (i + 1)));

			*oncoming = (topBits | (bottomBits << 1) | ((carPresent) ? 1 : 0)) & (uint32_t)0xFF;

			break;
		}
	}
}

static uint32_t CalculateDisplay(uint32_t* oncoming, uint32_t* intersect, uint32_t* outgoing, TrafficLightState_t lightState, bool carPresent )
{
	if (lightState == Green)
	{
		// shift everything along evenly
		*outgoing = ( (   (*outgoing) << 1) | (((*intersect) >>2) & 0b001) ) & (uint32_t)0xFF;
		*intersect = ( ( (*intersect) << 1) | (((*oncoming) >> 7) & 0x01 ) ) & (uint32_t)0b111;
		*oncoming = (((*oncoming) << 1) | (carPresent ? 1 : 0)) & (uint32_t)0xFF;

	}
	else if (lightState == Yellow)
	{
		// shift the outgoing traffic, shifting in top of the intersect
		// shift the intersect, adding a zero in the LSB
		*outgoing = ( (   (*outgoing) << 1) | (((*intersect) >>2) & 0b001) ) & (uint32_t)0xFF;
		*intersect = ((*intersect) << 1) & (uint32_t)0b111;
		// shift the incoming, this is hard
		ShiftOncomingWhenStopped(oncoming, carPresent);
	}
	else // red
	{
		// shift outgoing evenly, adding zeros in
		*outgoing = (*outgoing) << 1;
		// shift outgoing the same as in red
		ShiftOncomingWhenStopped(oncoming, carPresent);
	}

	uint32_t displaySequence =
					((*oncoming) << ONCOMING_SHIFT)		|
					((*intersect) << INTERSECT_SHIFT)		|
					(((uint32_t)lightState) << LIGHT_SHIFT)	|
					((*outgoing) << OUTGOING_SHIFT)		;

	return displaySequence;
}

void vDisplayTask( void* pvParameters )
{
	TrafficLightState_t light = 0;
	uint32_t oncoming = 0;
	uint32_t outgoing = 0;
	uint32_t intersect = 0;
	uint32_t displaySequence = 0;
	bool carPresent = false;

	while(1)
	{
		printf("Displaying\n");
		// check if the event has occurred,
		// if yes add a car
		if ( (xEventGroupGetBits(xEvent) & 1) == (1 << 0 ) )
		{
			xEventGroupClearBits(xEvent, (1 << 0) );
			carPresent = true;
			printf("Car Arrival\n");
		}

		if ( xSemaphoreTake(xLightMutex, (TickType_t)10) == pdTRUE)
		{
			light = (trafficLight.currentState);
			xSemaphoreGive(xLightMutex);
		}

		displaySequence = CalculateDisplay(&oncoming, &intersect, &outgoing, light, carPresent);

		ShiftReg_Update(displaySequence);

		carPresent = false;
		printf("Display end\n");
		vTaskDelay(TIME_PERIOD);
	}

}


