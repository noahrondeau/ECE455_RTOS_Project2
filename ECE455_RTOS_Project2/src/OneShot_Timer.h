#ifndef OneShot_Timer_h
#define OneShot_Timer_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"

#include "Error.h"
#include "Event.h"
#include "Task.h"

#include "config.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	TimerHandle_t            h___timer;
	TimerCallbackFunction_t  p_func__callback;  //void CallbackFunctionExample(TimerHandle_t h___timer);
	Event                    event;
	Task                     task___execute_callback_in_this_context;
} OneShot_Timer;

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS OneShot_Timer___Init                      (OneShot_Timer* p___timer, uint32_t u32___period_ms    , TimerCallbackFunction_t p_func__callback);
EXIT_STATUS OneShot_Timer___Change_Period_and_Restart (OneShot_Timer* p___timer, uint32_t u32___new_period_ms);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
