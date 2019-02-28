//==================================== interface ==========================================================================================
//  typedef struct  OneShot_Timer;
//  
//  EXIT_STATUS OneShot_Timer___Init                      (OneShot_Timer* p___timer, uint32_t u32___period_ms    , TimerCallbackFunction_t p_func__callback);
//  EXIT_STATUS OneShot_Timer___Change_Period_and_Restart (OneShot_Timer* p___timer, uint32_t u32___new_period_ms);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "OneShot_Timer.h"

#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"

#include "Error.h"
#include "Event.h"
#include "Task.h"

#include "config.h"

//------------------------------------ timer callback -------------------------------------------------------------------------------------
void INTERNAL___OneShot_Timer___timer_callback(TimerHandle_t h___timer)
{
	// This timer has just been triggered. That's why we are here.
	
	// Get the pointer to this timer's struct (saved as TimerID, during the call to xTimerCreate()).
	OneShot_Timer* p___timer = (OneShot_Timer*)pvTimerGetTimerID(h___timer);
	
	// Just trigger the event for this timer.
	// The task for this timer will then be responsible for executing the actual callback.
	// Done this way so that THIS callback never has the potential to block.
	EXIT_STATUS exit_status = Event___Trigger(&p___timer->event);
	if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to trigger timer's event: p___timer = %X, p___timer->h___timer = %X\n", p___timer, p___timer->h___timer);
	
	// Done.
}

//------------------------------------ task -----------------------------------------------------------------------------------------------
void INTERNAL___OneShot_Timer___Task(void* p___parameters)
{
	EXIT_STATUS exit_status;
	
	// Get the pointer to this timer's struct.
	OneShot_Timer* p___timer = (OneShot_Timer*)p___parameters;
	
	// Infinite loop.
	bool b___auto_reset = true;
	while (1)
	{
		// Wait for this timer's event to be triggered.
		exit_status = Event___WaitFor(&p___timer->event, b___auto_reset);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to wait for this timer's event.");
		
		// Execute the actual timer callback.
		// Executes within the context of this task.
		(*p___timer->p_func__callback)(p___timer->h___timer);
	}
}

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS OneShot_Timer___Init(OneShot_Timer* p___timer, uint32_t u32___period_ms, TimerCallbackFunction_t p_func__callback)
{
	EXIT_STATUS exit_status;
	
	// NULL parameter check.
	if ((p___timer == NULL) || (p_func__callback == NULL))
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter(s): p___timer = %X, p_func__callback = %X\n", p___timer, p_func__callback);
		return(EXIT_FAILURE);
	}
	
	// Init struct.
	p___timer->p_func__callback = p_func__callback;
	
	// Init timer's event.
	exit_status = Event___Init(&p___timer->event);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init this timer's event.\n");
		return(exit_status);
	}
	
	// Init timer's task.
	exit_status = Task___Init(&p___timer->task___execute_callback_in_this_context, INTERNAL___OneShot_Timer___Task, configMINIMAL_STACK_SIZE, (void*)p___timer, 1);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init this timer's task.\n");
		return(exit_status);
	}
	
	// Create the software timer used.
	UBaseType_t x___auto_reload = pdFALSE;
	TimerHandle_t return_value = xTimerCreate("OneShot_Timer", pdMS_TO_TICKS(u32___period_ms), x___auto_reload, (void*)p___timer, INTERNAL___OneShot_Timer___timer_callback);
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

EXIT_STATUS OneShot_Timer___Change_Period_and_Restart(OneShot_Timer* p___timer, uint32_t u32___new_period_ms)
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
