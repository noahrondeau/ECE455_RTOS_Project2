#ifndef Timer_h
#define Timer_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"

#include "Error.h"

#include "config.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	TimerHandle_t  h___timer;
} Timer;

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Timer___Init                      (Timer* p___timer, uint32_t u32___period_ms    , TimerCallbackFunction_t p_func__callback, const char* const pz___timer_name);
EXIT_STATUS Timer___Change_Period_and_Restart (Timer* p___timer, uint32_t u32___new_period_ms);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
