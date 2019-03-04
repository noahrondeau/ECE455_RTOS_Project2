#ifndef MessageChannel_h
#define MessageChannel_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "queue.h"

#include "config.h"

//------------------------------------ data-types -----------------------------------------------------------------------------------------
typedef struct
{
	QueueHandle_t  h___queue;
} MessageChannel;

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS MessageChannel___Create  (MessageChannel* p___messageChannel, size_t message_size);
EXIT_STATUS MessageChannel___Send    (MessageChannel* p___messageChannel, const void* const P___MSG);
EXIT_STATUS MessageChannel___Receive (MessageChannel* p___messageChannel, void* const p___msg);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
