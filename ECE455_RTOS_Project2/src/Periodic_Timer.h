#ifndef Periodic_Timer_h
#define Periodic_Timer_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"

#include "config.h"
#include "Error.h"
#include "Mutex.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	Mutex                    mutex;
	TimerHandle_t            h___timer;
	TickType_t               x___current_period___ticks;
	TickType_t               x___new_period___ticks;
	TimerCallbackFunction_t  p_func__callback;  //void CallbackFunctionExample(TimerHandle_t h___timer);
} Periodic_Timer;

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Periodic_Timer___Init          (Periodic_Timer* p___timer, uint32_t u32___period_ms       , TimerCallbackFunction_t p_func__callback, const char* const pz___timer_name);
EXIT_STATUS Periodic_Timer___Change_Period (Periodic_Timer* p___timer, TickType_t x___new_period_ticks);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
