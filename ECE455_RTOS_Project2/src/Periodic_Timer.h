#ifndef Periodic_Timer_h
#define Periodic_Timer_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"

#include "Error.h"
#include "Mutex.h"
#include "Event.h"
#include "Task.h"

#include "config.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	TimerHandle_t            h___timer;
	TickType_t               x___current_period___ticks;
	struct
	{
		Mutex                  mutex;
		TickType_t             x___new_period___ticks;
	}                        protect;
	TimerCallbackFunction_t  p_func__callback;  //void CallbackFunctionExample(TimerHandle_t h___timer);
	Event                    event;
	Task                     task___execute_callback_in_this_context;
} Periodic_Timer;

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Periodic_Timer___Init          (Periodic_Timer* p___timer, uint32_t u32___period_ms       , TimerCallbackFunction_t p_func__callback);
EXIT_STATUS Periodic_Timer___Change_Period (Periodic_Timer* p___timer, TickType_t x___new_period_ticks);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
