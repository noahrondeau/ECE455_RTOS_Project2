//==================================== interface ==========================================================================================
//  EXIT_STATUS Task1___potentiometer_reader___Init(ADC_TypeDef* which_ADC_using, uint32_t u32___ms_between_ADC_reads);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <MessageChannel.h>
#include "Task1___potentiometer_reader.h"

#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx_adc.h"
#include "FreeRTOS.h"

#include "Task.h"
#include "ADC.h"

//------------------------------------ globals --------------------------------------------------------------------------------------------
ADC_TypeDef*  g___which_ADC_using;
TickType_t    gx___ticks_between_ADC_reads;

extern MessageChannel  g___message_channel___flow_rate_1_2;
extern MessageChannel  g___message_channel___flow_rate_1_3;

//------------------------------------ task -----------------------------------------------------------------------------------------------
void Task1___potentiometer_reader(void* p___parameters)
{
	EXIT_STATUS exit_status;
	
	static float fp32___bottom = (float)ADC_VALUE_MAX;
	float fp32___top;
	float fp32___traffic_flow_rate___between_0_and_1;
	void* p;
	
	uint16_t u16___adc_value;
	
	while(1)
	{
		// Read in a value from the ADC.
		u16___adc_value = ADC___Read(g___which_ADC_using);
		
		// Convert the value to a float between 0-1 (others will use this).
		fp32___top = (float)u16___adc_value;
		fp32___traffic_flow_rate___between_0_and_1 = fp32___top / fp32___bottom;
		if (fp32___traffic_flow_rate___between_0_and_1 > 1.0) fp32___traffic_flow_rate___between_0_and_1 = 1.0;
		
		// Send out this traffic flow rate value to everyone who needs it (via messenger pigeon).
		p = (void*)&fp32___traffic_flow_rate___between_0_and_1;
		
		exit_status = MessageChannel___Send(&g___message_channel___flow_rate_1_2, p);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to send message FROM task1 TO task2.\n");
		
		exit_status = MessageChannel___Send(&g___message_channel___flow_rate_1_3, p);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to send message FROM task1 TO task3.\n");
		
		// Wait before doing next ADC read.
		vTaskDelay(gx___ticks_between_ADC_reads);
	}
}

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Task1___potentiometer_reader___Init(ADC_TypeDef* which_ADC_using, uint32_t u32___ms_between_ADC_reads)
{
	// Init globals.
	g___which_ADC_using = which_ADC_using;
	gx___ticks_between_ADC_reads = pdMS_TO_TICKS(u32___ms_between_ADC_reads);
	
	// Create the task.
	EXIT_STATUS exit_status = Task___Init(NULL, Task1___potentiometer_reader, configMINIMAL_STACK_SIZE, NULL, 1);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to create task1.\n");
		return(exit_status);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
