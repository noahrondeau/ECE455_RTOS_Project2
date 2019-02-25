//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// - These are the "Messenger Pigeons" used to send messages from one task to another.
// - For more information, see "Messenger_Pigeon.c".
//
// - NOTE***: Need to be init'd before tasks are run (easiest to just do before the task scheduler is started).
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//==================================== interface ==========================================================================================
//  Messenger_Pigeon  g___messenger_pigeon___FROM_task1_TO_task2___fp32___traffic_flow_rate___between_0_and_1;
//  Messenger_Pigeon  g___messenger_pigeon___FROM_task1_TO_task3___fp32___traffic_flow_rate___between_0_and_1;
//  
//  EXIT_STATUS Init_Our_Lovely_Messenger_Pigeons(void);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "messenger_pigeons.h"

#include <stdlib.h>

#include "defs.h"
#include "Error.h"
#include "Messenger_Pigeon.h"

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Init_Our_Lovely_Messenger_Pigeons(void)
{
	EXIT_STATUS exit_status;
	
	// Init our lovely messenger pigeons.
	exit_status = Messenger_Pigeon___Create(&g___messenger_pigeon___FROM_task1_TO_task2___fp32___traffic_flow_rate___between_0_and_1, sizeof(float));
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init pigeon FROM task1 TO task2.\n");
		return(exit_status);
	}
	
	exit_status = Messenger_Pigeon___Create(&g___messenger_pigeon___FROM_task1_TO_task3___fp32___traffic_flow_rate___between_0_and_1, sizeof(float));
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init pigeon FROM task1 TO task3.\n");
		return(exit_status);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
