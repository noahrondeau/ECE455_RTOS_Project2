//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  - This "Periodic_Timer" is based on "FreeRTOS software timers".
//  - Alterations have be made to how changing the timer period is done.
//
//  - When "FreeRTOS software timers" change the timer period, the new period is based on when xTimerChangePeriod() was called. Ultimately,
//    the timer is reset without being triggered, every time xTimerChangePeriod() is called.
//  - This could lead to the situation where the timer is never actually triggered though.
//
//  - Instead, our "Periodic_Timer" only actually makes the period change once the current period has been triggered.
//  - Removes the possibility of timer starvation.
//  - Changes to timer period are always 1 period behind though. Should still be ok though.
//  - If multiple timer period change requests are made before the current period is triggered, the new timer period will be the latest
//    change request.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//==================================== interface ==========================================================================================
//  typedef struct  Periodic_Timer;
//  
//  EXIT_STATUS Periodic_Timer___Init          (Periodic_Timer* p___timer, uint32_t u32___period_ms       , TimerCallbackFunction_t p_func__callback, const char* const pz___timer_name)
//  EXIT_STATUS Periodic_Timer___Change_Period (Periodic_Timer* p___timer, TickType_t x___new_period_ticks);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Periodic_Timer.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "timers.h"

#include "defs.h"
#include "Error.h"
#include "Mutex.h"

//------------------------------------ timer callback -------------------------------------------------------------------------------------
void INTERNAL___Periodic_Timer___timer_callback_function(TimerHandle_t h___timer)
{
	// This timer has just been triggered. That's why we are here.
	
	// Get the pointer to this timer's struct (saved as TimerID, during the call to xTimerCreate()).
	Periodic_Timer* p___timer = (Periodic_Timer*)pvTimerGetTimerID(h___timer);
	
	// Grab the mutex.
	EXIT_STATUS exit_status = Mutex___WaitFor(&p___timer->mutex);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to wait for the mutex.\n");
	}
	
	// If the period for this timer needs changing, do that now.
	if (p___timer->x___new_period___ticks != p___timer->x___current_period___ticks)
	{
		printf("We are changing the period\n");
		// Change the timer period.
		BaseType_t return_value = xTimerChangePeriod(h___timer, p___timer->x___new_period___ticks, portMAX_DELAY);
		if (return_value == pdFAIL)
		{
			Error(FUNCTION_SIGNATURE, "Request to change timer's period was not successfully written to the timer command queue.\n");
		}
		if (return_value != pdPASS)
		{
			Error(FUNCTION_SIGNATURE, "Unknown failure: xTimerChangePeriod()\n");
		}
		
		// Save the new period as current.
		p___timer->x___current_period___ticks = p___timer->x___new_period___ticks;
	}
	
	// Release the mutex.
	exit_status = Mutex___Release(&p___timer->mutex);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to release the mutex.\n");
	}
	
	// Now, proceed as normal to the timer's user-supplied callback function.
	(*p___timer->p_func__callback)(h___timer);
	printf("timer callback success\n");
	// Done.
}

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Periodic_Timer___Init(Periodic_Timer* p___timer, uint32_t u32___period_ms, TimerCallbackFunction_t p_func__callback, const char* const pz___timer_name)
{
	// NULL parameter check.
	if ((p___timer == NULL) || (p_func__callback == NULL) || (pz___timer_name == NULL))
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter(s): p___timer = %X, p_func__callback = %X, pz___timer_name = %X\n", p___timer, p_func__callback, pz___timer_name);
		return(EXIT_FAILURE);
	}
	
	// Set values.
	p___timer->x___current_period___ticks = pdMS_TO_TICKS(u32___period_ms);
	p___timer->p_func__callback = p_func__callback;
	
	// Init mutex.
	EXIT_STATUS exit_status = Mutex___Init(&p___timer->mutex);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init the mutex.\n");
		return(exit_status);
	}
	
	// Create the software timer used.
	TimerHandle_t return_value = xTimerCreate(pz___timer_name, p___timer->x___current_period___ticks, pdTRUE, (void*)p___timer, INTERNAL___Periodic_Timer___timer_callback_function);
	if (return_value == NULL)
	{
		Error(FUNCTION_SIGNATURE, "Insufficient heap memory to create software timer.\n");
		return(EXIT_FAILURE);
	}
	p___timer->h___timer = return_value;

	//Start Timer
	BaseType_t temp = xTimerStart(p___timer->h___timer,portMAX_DELAY);
	
	printf("Timer initialized\n");
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Periodic_Timer___Change_Period(Periodic_Timer* p___timer, TickType_t x___new_period_ticks)
{
	EXIT_STATUS exit_status;
	
	// NULL parameter check.
	if (p___timer == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter.\n");
		return(EXIT_FAILURE);
	}
	
	// Grab the mutex.
	exit_status = Mutex___WaitFor(&p___timer->mutex);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to wait for the mutex.\n");
		return(exit_status);
	}
	
	// Set the value for the new period.
	p___timer->x___new_period___ticks = x___new_period_ticks;
	
	// Release the mutex.
	exit_status = Mutex___Release(&p___timer->mutex);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to release the mutex.\n");
		return(exit_status);
	}
	 printf("Timer period change requested\n");
	// Done.
	return(EXIT_SUCCESS);
}
