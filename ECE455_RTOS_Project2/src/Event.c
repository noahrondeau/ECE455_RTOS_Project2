//==================================== interface ==========================================================================================
//  typedef struct Event;
//  
//  EXIT_STATUS Event___Init         (Event* p___event);
//  EXIT_STATUS Event___Trigger      (Event* p___event);
//  EXIT_STATUS Event___Is_Triggered (Event* p___event, bool* pb___is_triggered);
//  EXIT_STATUS Event___WaitFor      (Event* p___event, bool b___auto_reset);
//  EXIT_STATUS Event___Reset        (Event* p___event);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Event.h"

#include <stdlib.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "event_groups.h"

#include "Error.h"
#include "config.h"

//------------------------------------ task -----------------------------------------------------------------------------------------------
EXIT_STATUS Event___Init(Event* p___event)
{
	// NULL parameter check.
	if (p___event == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Create the event.
	EventGroupHandle_t return_value = xEventGroupCreate();
	if (return_value == NULL)
	{
		Error(FUNCTION_SIGNATURE, "xEventGroupCreate() failure; Insufficient memory on heap.\n");
		return(EXIT_FAILURE);
	}
	p___event->h___event_group = return_value;
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Event___Trigger(Event* p___event)
{
	// NULL parameter check.
	if (p___event == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Trigger the event.
	xEventGroupSetBits(p___event->h___event_group, 1);
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Event___Is_Triggered (Event* p___event, bool* pb___is_triggered)
{
	// NULL parameter check.
	if ((p___event == NULL) || (pb___is_triggered == NULL))
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter(s): p___event = %X, pb___is_triggered = %X\n", p___event, pb___is_triggered);
		return(EXIT_FAILURE);
	}
	
	// Is this event triggered?
	EventBits_t return_value = xEventGroupGetBits(p___event->h___event_group);
	*pb___is_triggered = (return_value & 1) ? true : false;
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Event___WaitFor(Event* p___event, bool b___auto_reset)
{
	// NULL parameter check.
	if (p___event == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Wait for this event.
	BaseType_t x___auto_reset = (b___auto_reset == 0) ? pdFALSE : pdTRUE;
	BaseType_t x___wait_for_all_bits = pdFALSE;
	xEventGroupWaitBits(p___event->h___event_group, 1, x___auto_reset, x___wait_for_all_bits, portMAX_DELAY);
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Event___Reset(Event* p___event)
{
	// NULL parameter check.
	if (p___event == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Reset this event.
	xEventGroupClearBits(p___event->h___event_group, 1);
	
	// Done.
	return(EXIT_SUCCESS);
}
