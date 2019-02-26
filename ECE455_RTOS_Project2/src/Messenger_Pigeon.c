//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// - "Messenger Pigeon" is used to send messages from one task to another.
// - Based on FreeRTOS "queue".
// - Sized so that there can only be 1 message at a time
// - Send is immediate. Replaces any old value with the new.
// - Receive blocks until receivers something.
// - Message size set (generally according to datatype) during init.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//==================================== interface ==========================================================================================
//  typedef struct Messenger_Pigeon;
//  
//  EXIT_STATUS Messenger_Pigeon___Create  (Messenger_Pigeon* p___messenger_pigeon,  size_t message_size);
//  EXIT_STATUS Messenger_Pigeon___Send    (Messenger_Pigeon* p___messenger_pigeon, const void* const P___MSG);
//  EXIT_STATUS Messenger_Pigeon___Receive (Messenger_Pigeon* p___messenger_pigeon,       void* const p___msg);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Messenger_Pigeon.h"

#include <stdlib.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "config.h"
#include "Error.h"

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Messenger_Pigeon___Create(Messenger_Pigeon* p___messenger_pigeon, size_t message_size)
{
	// NULL parameter check.
	if (p___messenger_pigeon == NULL)
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter\n");
		return(EXIT_FAILURE);
	}
	
	// Create the queue used.
	QueueHandle_t return_value = xQueueCreate(1, message_size);
	if (return_value == NULL)
	{
		Error(FUNCTION_SIGNATURE, "xQueueCreate() failure; insufficient heap memory; message_size = %u\n", message_size);
		return(EXIT_FAILURE);
	}
	
	// Save value into the struct.
	p___messenger_pigeon->h___queue = return_value;
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Messenger_Pigeon___Send(Messenger_Pigeon* p___messenger_pigeon, const void* const P___MSG)
{
	// NULL parameter check.
	if ((p___messenger_pigeon == NULL) || (P___MSG == NULL))
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter(s): p___messenger_pigeon = %X, P___MSG = %X\n", p___messenger_pigeon, P___MSG);
		return(EXIT_FAILURE);
	}
	
	// Send the messenger pigeon!
	BaseType_t return_value = xQueueOverwrite(p___messenger_pigeon->h___queue, P___MSG);
	if (return_value != pdPASS)
	{
		Error(FUNCTION_SIGNATURE, "Unknown failure: xQueueOverwrite()\n");
		return(EXIT_FAILURE);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}

EXIT_STATUS Messenger_Pigeon___Receive(Messenger_Pigeon* p___messenger_pigeon, void* const p___msg)
{
	// NULL parameter check.
	if ((p___messenger_pigeon == NULL) || (p___msg == NULL))
	{
		Error(FUNCTION_SIGNATURE, "NULL parameter(s): p___messenger_pigeon = %X, p___msg = %X\n", p___messenger_pigeon, p___msg);
		return(EXIT_FAILURE);
	}
	
	// Receive a message via messenger pigeon!
	BaseType_t return_value = xQueueReceive(p___messenger_pigeon->h___queue, p___msg, portMAX_DELAY);
	if (return_value == errQUEUE_EMPTY)
	{
		Error(FUNCTION_SIGNATURE, "Failed to receive from the queue. Was empty. This should NOT happen as I should have blocked until there was something.\n");
		return(EXIT_FAILURE);
	}
	if (return_value != pdPASS)
	{
		Error(FUNCTION_SIGNATURE, "Unknown failure: xQueueReceive()\n");
		return(EXIT_FAILURE);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
