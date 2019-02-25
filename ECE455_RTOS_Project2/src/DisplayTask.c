/*
 * DisplayTask.c
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#include "DisplayTask.h"
#include "ShiftReg.h"

void vDisplayTask( void* pvParameters )
{
	uint32_t light;
	uint32_t oncoming;
	uint32_t outgoing;
	uint32_t intersect;
	uint32_t displaySequence;

	while(1)
	{
		// TEMP: this should be replaced with taking and giving a mutex

		if ( xSemaphoreTake( xBitFieldMutex, (TickType_t)100 ) == pdTRUE );
		{
			light		= (uint32_t)lightState;
			oncoming	= oncomingTrafficBitField;
			outgoing	= outgoingTrafficBitField;
			intersect	= intersectionTrafficBitField;

			xSemaphoreGive( xBitFieldMutex );

			displaySequence =
					(oncoming << ONCOMING_SHIFT)	|
					(intersect << INTERSECT_SHIFT)	|
					(light << LIGHT_SHIFT)			|
					(outgoing << OUTGOING_SHIFT)	;

			ShiftReg_Update(displaySequence);
		}

		vTaskDelay(10);
	}

}
