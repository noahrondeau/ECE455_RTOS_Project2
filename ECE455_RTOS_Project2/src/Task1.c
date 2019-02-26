//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Task 1: Traffic flow adjustment task
//
// - NOTE: Needs to be init'd before other tasks are run (easiest to just do before the task scheduler is started).
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//==================================== interface ==========================================================================================
//  EXIT_STATUS Task1___Init(ADC_TypeDef* which_ADC_using, uint32_t u32___ms_between_ADC_reads);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Task1.h"

#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx_adc.h"
#include "FreeRTOS.h"

#include "defs.h"
#include "priorities.h"
#include "Task.h"
#include "ADC.h"
#include "Messenger_Pigeon.h"
#include "messenger_pigeons.h"

//------------------------------------ globals --------------------------------------------------------------------------------------------
ADC_TypeDef*  g___which_ADC_using;
TickType_t    gx___ticks_between_ADC_reads;

//------------------------------------ task -----------------------------------------------------------------------------------------------
void Task1(void* p___parameters)
{
	EXIT_STATUS exit_status;
	
	//printf("Start Task 1\n");

	static float fp32___bottom = (float)ADC_VALUE_MAX;
	float fp32___top;
	float fp32___traffic_flow_rate___between_0_and_1;
	void* p;
	
	uint16_t u16___adc_value;
	
	while(1)
	{
		//printf("Task 1 Loop Start\n");
		// Read in a value from the ADC.
		u16___adc_value = ADC___Read(g___which_ADC_using);
		
		//printf("ADC Value Unsigned from Task 1: %d\n", u16___adc_value);

		// Convert the value to a float between 0-1 (others will use this).
		fp32___top = (float)u16___adc_value;
		fp32___traffic_flow_rate___between_0_and_1 = fp32___top / fp32___bottom;
		if (fp32___traffic_flow_rate___between_0_and_1 > 1.0) fp32___traffic_flow_rate___between_0_and_1 = 1.0;
		
		//printf("ADC normalized float from Task 1: %f", fp32___traffic_flow_rate___between_0_and_1);

		// Send out this traffic flow rate value to everyone who needs it (via messenger pigeon).
		p = (void*)&fp32___traffic_flow_rate___between_0_and_1;
		
		exit_status = Messenger_Pigeon___Send(&g___messenger_pigeon___FROM_task1_TO_task2___fp32___traffic_flow_rate___between_0_and_1, p);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to send messenger pigeon FROM task1 TO task2.\n");
		
		exit_status = Messenger_Pigeon___Send(&g___messenger_pigeon___FROM_task1_TO_task3___fp32___traffic_flow_rate___between_0_and_1, p);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to send messenger pigeon FROM task1 TO task3.\n");
		
		// Wait before doing next ADC read.
		//printf("Task 1 loop end\n");
		vTaskDelay(gx___ticks_between_ADC_reads);
	}
}

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Task1___Init(ADC_TypeDef* which_ADC_using, uint32_t u32___ms_between_ADC_reads)
{
	// Init globals.
	g___which_ADC_using = which_ADC_using;
	gx___ticks_between_ADC_reads = pdMS_TO_TICKS(u32___ms_between_ADC_reads);
	
	// Create the task.
	EXIT_STATUS exit_status = Task___Init(NULL, Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, PRIORITY___regular);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to create task1.\n");
		return(exit_status);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
