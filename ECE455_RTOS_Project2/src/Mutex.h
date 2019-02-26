#ifndef Mutex_h
#define Mutex_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "semphr.h"

#include "defs.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	SemaphoreHandle_t h___semaphore;
} Mutex;

//------------------------------------ task -----------------------------------------------------------------------------------------------
EXIT_STATUS Mutex___Init    (Mutex* p___mutex);
EXIT_STATUS Mutex___WaitFor (Mutex* p___mutex);
EXIT_STATUS Mutex___Release (Mutex* p___mutex);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif