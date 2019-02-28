#ifndef Event_h
#define Event_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <stdbool.h>
#include "FreeRTOS.h"
#include "event_groups.h"

#include "config.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	EventGroupHandle_t h___event_group;
} Event;

//------------------------------------ task -----------------------------------------------------------------------------------------------
EXIT_STATUS Event___Init         (Event* p___event);
EXIT_STATUS Event___Trigger      (Event* p___event);
EXIT_STATUS Event___Is_Triggered (Event* p___event, bool* pb___is_triggered);
EXIT_STATUS Event___WaitFor      (Event* p___event, bool b___auto_reset);
EXIT_STATUS Event___Reset        (Event* p___event);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
