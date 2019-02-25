#ifndef Messenger_Pigeon_h
#define Messenger_Pigeon_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "queue.h"

#include "defs.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	QueueHandle_t  h___queue;
} Messenger_Pigeon;

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Messenger_Pigeon___Create  (Messenger_Pigeon* p___messenger_pigeon, size_t message_size);
EXIT_STATUS Messenger_Pigeon___Send    (Messenger_Pigeon* p___messenger_pigeon, const void* const P___MSG);
EXIT_STATUS Messenger_Pigeon___Receive (Messenger_Pigeon* p___messenger_pigeon, void* const p___msg);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
