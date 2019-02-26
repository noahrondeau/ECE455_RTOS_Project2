//==================================== interface ==========================================================================================
//  typedef struct  Timer;
//  
//  EXIT_STATUS Timer___Init                      (Timer* p___timer, uint32_t u32___period_ms    , TimerCallbackFunction_t p_func__callback, const char* const pz___timer_name);
//  EXIT_STATUS Timer___Change_Period_and_Restart (Timer* p___timer, uint32_t u32___new_period_ms);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Timer.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "timers.h"

#include "Error.h"

#include "config.h"

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Timer___Init(Timer* p___timer, uint32_t u32___period_ms, TimerCallbackFunction_t p_func__callback, const char* const pz___timer_name)
{
	// NULL parameter check.
	if ((p___timer == NULL) || (p_func__callback == NULL) || (pz___timer_name == NULL))
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter(s): p___timer = %X, p_func__callback = %X, pz___timer_name = %X\n", p___timer, p_func__callback, pz___timer_name);
		return(EXIT_FAILURE);
	}
	
	// Create the software timer used.
	TimerHandle_t return_value = xTimerCreate(pz___timer_name, pdMS_TO_TICKS(u32___period_ms), pdFALSE, (void*)p___timer, p_func__callback);
	if (return_value == NULL)
	{
		Error(FUNCTION_SIGNATURE, "Insufficient heap memory to create software timer.\n");
		return(EXIT_FAILURE);
	}
	p___timer->h___timer = return_value;

	// Start the timer.
	BaseType_t return_value2 = xTimerStart(p___timer->h___timer, portMAX_DELAY);
	if (return_value2 == pdFAIL)
	{
		Error(FUNCTION_SIGNATURE, "Failed to start timer in time. Should NOT have happened, because we wait infinitely for this to happen.\n");
		return(EXIT_FAILURE);
	}
	if (return_value2 != pdPASS)
	{
		Error(FUNCTION_SIGNATURE, "Unknown failure: xTimerStart()\n");
		return(EXIT_FAILURE);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Timer___Change_Period_and_Restart(Timer* p___timer, uint32_t u32___new_period_ms)
{
	// NULL parameter check.
	if (p___timer == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Change the timer period.
	BaseType_t return_value = xTimerChangePeriod(p___timer->h___timer, pdMS_TO_TICKS(u32___new_period_ms), portMAX_DELAY);
	if (return_value == pdFAIL)
	{
		Error(FUNCTION_SIGNATURE, "Request to change timer's period was not successfully written to the timer command queue.\n");
		return(EXIT_FAILURE);
	}
	if (return_value != pdPASS)
	{
		Error(FUNCTION_SIGNATURE, "Unknown failure: xTimerChangePeriod()\n");
		return(EXIT_FAILURE);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
