//==================================== interface ==========================================================================================
//  typedef struct Mutex;
//  
//  EXIT_STATUS Mutex___Init    (Mutex* p___mutex);
//  EXIT_STATUS Mutex___WaitFor (Mutex* p___mutex);
//  EXIT_STATUS Mutex___Release (Mutex* p___mutex);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Mutex.h"

#include <stdlib.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "Error.h"
#include "config.h"

//------------------------------------ task -----------------------------------------------------------------------------------------------
EXIT_STATUS Mutex___Init(Mutex* p___mutex)
{
	// NULL parameter check.
	if (p___mutex == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Create the mutex.
	SemaphoreHandle_t return_value = xSemaphoreCreateMutex();
	if (return_value == NULL)
	{
		Error(FUNCTION_SIGNATURE, "xSemaphoreCreateMutex() failure; Insufficient memory on heap.\n");
		return(EXIT_FAILURE);
	}
	p___mutex->h___semaphore = return_value;
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Mutex___WaitFor(Mutex* p___mutex)
{
	// NULL parameter check.
	if (p___mutex == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Wait for this mutex.
	//
	// NOTE:
	// - To block indefinitely (via "portMAX_DELAY"), needs "INCLUDE_vTaskSuspend" set to 1 in "FreeRTOSConfig.h"
	//
	BaseType_t return_value = xSemaphoreTake(p___mutex->h___semaphore, portMAX_DELAY);
	if (return_value == pdFAIL)
	{
		Error(FUNCTION_SIGNATURE, "Failure to wait for the mutex. Seems that it timed out. Set to wait indefinitely though, so this should NOT have happened.\n");
		return(EXIT_FAILURE);
	}
	if (return_value != pdPASS)
	{
		Error(FUNCTION_SIGNATURE, "Unknown failure: xSemaphoreTake()\n");
		return(EXIT_FAILURE);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Mutex___Release(Mutex* p___mutex)
{
	// NULL parameter check.
	if (p___mutex == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Release this mutex.
	BaseType_t return_value = xSemaphoreGive(p___mutex->h___semaphore);
	if (return_value == pdFAIL)
	{
		Error(FUNCTION_SIGNATURE, "Failed to release this mutex. Was not holding it.\n");
		return(EXIT_FAILURE);
	}
	if (return_value != pdPASS)
	{
		Error(FUNCTION_SIGNATURE, "Unknown failure: xSemaphoreGive()\n");
		return(EXIT_FAILURE);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
