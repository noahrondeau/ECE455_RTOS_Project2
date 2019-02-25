#ifndef Task_h
#define Task_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "config.h"
#include "defs.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	TaskHandle_t h___task;
} Task;

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Task___Init(Task* p___task, TaskFunction_t task_function, const char* const PZ___TASK_NAME, unsigned short stack_size, void* p___parameters, UBaseType_t priority);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
