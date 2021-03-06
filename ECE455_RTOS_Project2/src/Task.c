//==================================== interface ==========================================================================================
//  typedef struct Task;
//  
//  EXIT_STATUS  Task___Init(Task* p___task, TaskFunction_t task_function, unsigned short stack_size, void* p___parameters, UBaseType_t priority);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Task.h"

#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Error.h"
#include "config.h"

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Task___Init(Task* p___task, TaskFunction_t task_function, unsigned short stack_size, void* p___parameters, UBaseType_t priority)
{
	// Create the task.
	TaskHandle_t h___task;
	BaseType_t return_value = xTaskCreate(task_function, "Task", stack_size, p___parameters, priority, &h___task);
	if (return_value == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
	{
		Error(FUNCTION_SIGNATURE, "Task could not be created: insufficient heap memory; stack size = %u\n", stack_size);
		return(EXIT_FAILURE);
	}
	if (return_value != pdPASS)
	{
		Error(FUNCTION_SIGNATURE, "Unknown failure: xTaskCreate(); stack size = %u\n", stack_size);
		return(EXIT_FAILURE);
	}
	if (p___task != NULL)
	{
		p___task->h___task = h___task;
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
