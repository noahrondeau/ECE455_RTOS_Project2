/*
 * DisplayTask.c
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#include "DisplayTask.h"
#include "ShiftReg.h"

// globals defined in main.c
extern TrafficLight_t trafficLight;
extern SemaphoreHandle_t xLightMutex;
extern EventGroupHandle_t xEvent;

/* Algorithm to accumulate traffic behind the stop line when light is red or yellow
 * This is a MSB-first search for the first 0 in the field, followed by extracting
 * the top bits (that won't be moving) and the bottom bits that are shifted by 1 to the left
 * into the position of the 0. The new car is added if there is one.
 */
static void ShiftOncomingWhenStopped(uint32_t* oncoming, bool carPresent)
{
	uint32_t searchMask = (uint32_t)(0b10000000);
	uint32_t shiftMask	= (uint32_t)(0b11111111);
	// start from bit 7, and look for most significant 0,
	// and move everything else up
	for ( int i = 0; i < 8; i++)
	{
		// if we find a 0
		if ( ((*oncoming) & (searchMask >> i)) == 0 )
		{
			//extract top and bottom bits by creating bit masks
			uint32_t topBits = (*oncoming) & (shiftMask << (8-i));
			uint32_t bottomBits = ((*oncoming) & (shiftMask >> (i + 1)));

			//recombine the top bits and bottom bits
			*oncoming = (topBits | (bottomBits << 1) | ((carPresent) ? 1 : 0)) & (uint32_t)0xFF;

			// we only do this once, other wise all the traffic would accumulate in one cycle
			// so break out of the loop
			break;
		}
	}
}

/* Calculates the bitfield sequences representing car locations for the current display refresh event (ever 1 second) */
static uint32_t CalculateDisplay(uint32_t* oncoming, uint32_t* intersect, uint32_t* outgoing, TrafficLightState_t lightState, bool carPresent )
{
	if (lightState == Green)
	{
		// shift everything along evenly
		// for each bitfield, extract the MSB and place it in the LSB of the next field up
		// while shifting the entire field one to the left
		// the AND at the end is to set to zero all the rest of the fields that we don't care about
		*outgoing = ( (   (*outgoing) << 1) | (((*intersect) >>2) & 0b001) ) & (uint32_t)0xFF;
		*intersect = ( ( (*intersect) << 1) | (((*oncoming) >> 7) & 0x01 ) ) & (uint32_t)0b111;
		*oncoming = (((*oncoming) << 1) | (carPresent ? 1 : 0)) & (uint32_t)0xFF;

	}
	else if (lightState == Yellow)
	{
		// shift the outgoing traffic, shifting in top of the intersect
		// shift the intersect, adding a zero in the LSB
		// same logic as before, just not for the oncoming traffic
		*outgoing = ( (   (*outgoing) << 1) | (((*intersect) >>2) & 0b001) ) & (uint32_t)0xFF;
		*intersect = ((*intersect) << 1) & (uint32_t)0b111;
		// shift the incoming, this is hard
		ShiftOncomingWhenStopped(oncoming, carPresent);
	}
	else // red
	{
		// shift outgoing evenly, adding zeros in
		*outgoing = (*outgoing) << 1;
		// there are no more cars in the intersection anyway
		// *intersection = 0; // not really necessary, want to leave commented so
		// that we can see if there are errors


		// shift outgoing the same as in yellow
		ShiftOncomingWhenStopped(oncoming, carPresent);
	}

	// Create display sequence by combining all the bitfields
	uint32_t displaySequence =
					((*oncoming) << ONCOMING_SHIFT)		|
					((*intersect) << INTERSECT_SHIFT)		|
					(((uint32_t)lightState) << LIGHT_SHIFT)	|
					((*outgoing) << OUTGOING_SHIFT)		;

	return displaySequence;
}

/* This task maintains and updates the state of the system display output.
 * It gathers the state of the system at the display refresh times (every 1 second)
 * and then calculates the positions of the cars at those periods.
 */
void vDisplayTask( void* pvParameters )
{
	// State variables (local to the task)
	// these are bitfields representing the locations of the cars
	// they are uint32_t for simplicity, because they are combined
	// at every display event into a single uint32_t

	TrafficLightState_t light = 0; 	// traffic light val at moment of update
	uint32_t oncoming = 0;			// 8 bits of omcoming traffic
	uint32_t outgoing = 0;			// 8 bits of outgoing traffic
	uint32_t intersect = 0;			// 3 bits of intersection

	// bitfield to hold the output flashed to shift register
	uint32_t displaySequence = 0;

	// boolean for whether a car is present
	bool carPresent = false;

	while(1)
	{
		// check if the "car present" event happened since the last display refresh
		// if yes add a car
		if ( (xEventGroupGetBits(xEvent) & 1) == (1 << 0 ) )
		{
			xEventGroupClearBits(xEvent, (1 << 0) ); // clear event bit
			carPresent = true;
		}
		// else: no car will be added, just an empty slot

		// attempt to update traffic light state
		// timeout of 10 ticks means that if the traffic light control task
		// is still in the process of updating the light
		// just timeout and count it as "light hasn't changed yet" (keep old value)
		// the new value will take effect next time we refresh
		if ( xSemaphoreTake(xLightMutex, (TickType_t)10) == pdTRUE)
		{
			light = (trafficLight.currentState);
			xSemaphoreGive(xLightMutex);
		}

		// based on previous values of bitfields, and new values of light and carPresent,
		// calculate the next sequence (store in the same variables passed by reference
		// and output a bitfield containing the sequence to push to the shift register
		displaySequence = CalculateDisplay(&oncoming, &intersect, &outgoing, light, carPresent);

		// shift the bitfield onto the shift register
		ShiftReg_Update(displaySequence);

		carPresent = false;
		// delay for another second
		vTaskDelay(TIME_PERIOD);
	}

}


