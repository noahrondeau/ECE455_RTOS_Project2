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
//  EXIT_STATUS Periodic_Timer___Init          (Periodic_Timer* p___timer, uint32_t u32___period_ms       , TimerCallbackFunction_t p_func__callback)
//  EXIT_STATUS Periodic_Timer___Change_Period (Periodic_Timer* p___timer, TickType_t x___new_period_ticks);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Periodic_Timer.h"

#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"

#include "Error.h"
#include "Mutex.h"
#include "Event.h"
#include "Task.h"

#include "config.h"

//------------------------------------ timer callback -------------------------------------------------------------------------------------
void INTERNAL___Periodic_Timer___timer_callback(TimerHandle_t h___timer)
{
	// This timer has just been triggered. That's why we are here.
	
	// Get the pointer to this timer's struct (saved as TimerID, during the call to xTimerCreate()).
	Periodic_Timer* p___timer = (Periodic_Timer*)pvTimerGetTimerID(h___timer);
	
	// Just trigger the event for this timer.
	// The task for this timer will then be responsible for executing the actual callback.
	// Done this way so that THIS callback never has the potential to block.
	EXIT_STATUS exit_status = Event___Trigger(&p___timer->event);
	if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to trigger timer's event.\n");
	
	// Done.
}

//------------------------------------ task -----------------------------------------------------------------------------------------------
void INTERNAL___Periodic_Timer___Task(void* p___parameters)
{
	EXIT_STATUS exit_status;
	
	// Get the pointer to this timer's struct.
	Periodic_Timer* p___timer = (Periodic_Timer*)p___parameters;
	
	// Infinite loop.
	bool b___auto_reset = true;
	while (1)
	{
		// Wait for this timer's event to be triggered.
		exit_status = Event___WaitFor(&p___timer->event, b___auto_reset);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to wait for this timer's event.");
		
		
		// Grab the timer's mutex.
		exit_status = Mutex___WaitFor(&p___timer->protect.mutex);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to wait for the timer's mutex.\n");
		
		// If the period for this timer needs changing, do that now.
		if (p___timer->protect.x___new_period___ticks != p___timer->x___current_period___ticks)
		{
			// Change the timer period.
			BaseType_t return_value = xTimerChangePeriod(p___timer->h___timer, p___timer->protect.x___new_period___ticks, portMAX_DELAY);
			if (return_value == pdFAIL) Error(FUNCTION_SIGNATURE, "Request to change timer's period was not successfully written to the timer command queue.\n");
			if (return_value != pdPASS) Error(FUNCTION_SIGNATURE, "Unknown failure: xTimerChangePeriod()\n");
			
			// Save the new period as current.
			p___timer->x___current_period___ticks = p___timer->protect.x___new_period___ticks;
		}
		
		// Release the timer's mutex.
		exit_status = Mutex___Release(&p___timer->protect.mutex);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to release the timer's mutex.\n");
		
		
		// Execute the actual timer callback.
		// Executes within the context of this task.
		(*p___timer->p_func__callback)(p___timer->h___timer);
	}
}

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Periodic_Timer___Init(Periodic_Timer* p___timer, uint32_t u32___period_ms, TimerCallbackFunction_t p_func__callback)
{
	EXIT_STATUS exit_status;
	
	// NULL parameter check.
	if ((p___timer == NULL) || (p_func__callback == NULL))
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter(s): p___timer = %X, p_func__callback = %X\n", p___timer, p_func__callback);
		return(EXIT_FAILURE);
	}
	
	// Init struct.
	p___timer->x___current_period___ticks = pdMS_TO_TICKS(u32___period_ms);
	p___timer->protect.x___new_period___ticks = p___timer->x___current_period___ticks;
	p___timer->p_func__callback = p_func__callback;
	
	// Init timer's mutex.
	exit_status = Mutex___Init(&p___timer->protect.mutex);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init the timer's mutex.\n");
		return(exit_status);
	}
	
	// Init timer's event.
	exit_status = Event___Init(&p___timer->event);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init this timer's event.\n");
		return(exit_status);
	}
	
	// Init timer's task.
	exit_status = Task___Init(&p___timer->task___execute_callback_in_this_context, INTERNAL___Periodic_Timer___Task, configMINIMAL_STACK_SIZE, (void*)p___timer, 2);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init this timer's task.\n");
		return(exit_status);
	}
	
	// Create the software timer used.
	UBaseType_t x___auto_reload = pdTRUE;
	TimerHandle_t return_value = xTimerCreate("Periodic_Timer", p___timer->x___current_period___ticks, x___auto_reload, (void*)p___timer, INTERNAL___Periodic_Timer___timer_callback);
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
		Error(FUNCTION_SIGNATURE, "Failed to start timer in time. Should NOT have happened, because we wait indefinitely for this to happen.\n");
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
	exit_status = Mutex___WaitFor(&p___timer->protect.mutex);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to wait for the timer's mutex.\n");
		return(exit_status);
	}
	
	// Set the value for the new period.
	p___timer->protect.x___new_period___ticks = x___new_period_ticks;
	
	// Release the mutex.
	exit_status = Mutex___Release(&p___timer->protect.mutex);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to release the timer's mutex.\n");
		return(exit_status);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
