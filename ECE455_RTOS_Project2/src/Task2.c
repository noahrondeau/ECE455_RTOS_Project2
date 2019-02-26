//==================================== interface ==========================================================================================
//  EXIT_STATUS Task2___Init(float fp32___MIN_traffic_arrival_rate___cars_per_sec, float fp32___MAX_traffic_arrival_rate___cars_per_sec, float fp32___traffic_creation_threshold);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Task2.h"

#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "event_groups.h"

#include "defs.h"
#include "priorities.h"
#include "Task.h"
#include "Messenger_Pigeon.h"
#include "messenger_pigeons.h"
#include "Periodic_Timer.h"
#include "RNG.h"

#include "config.h"

//------------------------------------ globals --------------------------------------------------------------------------------------------
Periodic_Timer  g_timer___task2;
TickType_t      gx___MIN_traffic_arrival_rate___period___ticks;
TickType_t      gx___MAX_traffic_arrival_rate___period___ticks;
float           g_fp32___traffic_creation_threshold;
bool            g_b___pre_calculated___a_new_car;

//------------------------------------ timer callback -------------------------------------------------------------------------------------
void Task2___timer_callback(TimerHandle_t h___timer)
{
	printf("task2 timer call back top\n");
	// This is triggered at the rate of traffic creation.
	
	// Will I be creating a new car? (this will have been pre-calculated)
	if (g_b___pre_calculated___a_new_car)
	{
		// Yup, creating a new car.
		
		// Send the new car to task4.
		xEventGroupSetBits(xEvent, 1);  // Task4 is using bit0 of this event group.
		printf("car created \n");
	}
	
	// Pre-calculate, for next time, whether or not I'll be creating a new car.
	// Done ahead of time because the RNG might take a while.
	
	// Get a random number.
	uint32_t u32___top = RNG___Read();
	uint32_t u32___bottom = UINT32_MAX;
	
	// Convert random number to a float between 0 and 1;
	float fp32___top = u32___top;
	float fp32___bottom = u32___bottom;
	float fp32___random_number___between_0_and_1 = fp32___top / fp32___bottom;
	
	// Is my random number within the threshold for creating a new car?
	g_b___pre_calculated___a_new_car = (fp32___random_number___between_0_and_1 <= g_fp32___traffic_creation_threshold) ? 1 : 0;
	printf("task2 timer call back bottom\n");
}

//------------------------------------ task -----------------------------------------------------------------------------------------------
void Task2(void* p___parameters)
{
	// Receives "traffic flow rate" values (between 0 and 1) from Task1.
	
	EXIT_STATUS exit_status;
	
	float fp32___traffic_flow_rate___between_0_and_1;
	void* const P = (void*)&fp32___traffic_flow_rate___between_0_and_1;
	
	TickType_t x___delta = gx___MIN_traffic_arrival_rate___period___ticks - gx___MAX_traffic_arrival_rate___period___ticks;
	uint32_t u32___delta = x___delta;
	float fp32___delta = u32___delta;
	float fp32___temp;
	TickType_t x___temp;
	TickType_t x___new_period___ticks;

	while (1)
	{
		// Wait to receive a traffic flow rate value from task1, via messenger pigeon.
		exit_status = Messenger_Pigeon___Receive(&g___messenger_pigeon___FROM_task1_TO_task2___fp32___traffic_flow_rate___between_0_and_1, P);
		if (exit_status != 0) Error(FUNCTION_SIGNATURE, "Failed to receive messenger pigeon from task1.\n");
		
		// Based on the received "traffic flow rate", determine and set task2's timer.
		fp32___temp = (1 - fp32___traffic_flow_rate___between_0_and_1) * fp32___delta;
		x___temp = fp32___temp;
		x___new_period___ticks = gx___MAX_traffic_arrival_rate___period___ticks + x___temp;
		
		printf("task2 timer period change\n");
		exit_status = Periodic_Timer___Change_Period(&g_timer___task2, x___new_period___ticks);
		if (exit_status != 0)
		{
			Error(FUNCTION_SIGNATURE, "Failed to change the period for task2's timer.\n");
		}
	}
}

//------------------------------------ functions ------------------------------------------------------------------------------------------
intern TickType_t HELPER___Frequency_to_Period____X_per_sec____ticks(float fp32___X_per_sec)
{
	float       fp32___period___ms;
	uint32_t    u32___period___ms;
	TickType_t  x___period___ticks;
	
	// Calculate.
	if (fp32___X_per_sec <= 0.2)
	{
		fp32___X_per_sec = 0.2;
	}
	fp32___period___ms = 1.0 / (fp32___X_per_sec / 1000.0);
	u32___period___ms = fp32___period___ms;
	x___period___ticks = pdMS_TO_TICKS(u32___period___ms);
	
	// Done.
	return(x___period___ticks);
}

EXIT_STATUS Task2___Init(float fp32___MIN_traffic_arrival_rate___cars_per_sec, float fp32___MAX_traffic_arrival_rate___cars_per_sec, float fp32___traffic_creation_threshold)
{
	EXIT_STATUS exit_status;
	
	// For the "traffic arrival rates", make sure that MIN <= MAX.
	if (fp32___MIN_traffic_arrival_rate___cars_per_sec > fp32___MAX_traffic_arrival_rate___cars_per_sec)
	{
		Error(FUNCTION_SIGNATURE, "For the traffic arrivals rates, MIN > MAX. This should not be.\n");
		return(EXIT_FAILURE);
	}
	
	// Make sure the "traffic creation threshold" is not negative.
	if (fp32___traffic_creation_threshold < 0.0)
	{
		Error(FUNCTION_SIGNATURE, "The traffic creation threshold was negative.\n");
		return(EXIT_FAILURE);
	}
	
	// Calculate periods (in ticks), for the traffic arrival rates.
	gx___MIN_traffic_arrival_rate___period___ticks = HELPER___Frequency_to_Period____X_per_sec____ticks(fp32___MIN_traffic_arrival_rate___cars_per_sec);
	gx___MAX_traffic_arrival_rate___period___ticks = HELPER___Frequency_to_Period____X_per_sec____ticks(fp32___MAX_traffic_arrival_rate___cars_per_sec);
	
	// Init remaining globals.
	g_fp32___traffic_creation_threshold = fp32___traffic_creation_threshold;
	g_b___pre_calculated___a_new_car = 1;  // The first time thru may as well always get a new car :)
	
	// Init the timer.
	// May as well init the timer to the max "traffic arrival rate".
	exit_status = Periodic_Timer___Init(&g_timer___task2, gx___MAX_traffic_arrival_rate___period___ticks, Task2___timer_callback, "Task 2 timer");
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to init task2's timer.\n");
		return(exit_status);
	}
	
	// Create the task.
	exit_status = Task___Init(NULL, Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, PRIORITY___regular);
	if (exit_status != 0)
	{
		Error(FUNCTION_SIGNATURE, "Failed to create task2.\n");
		return(exit_status);
	}
	
	// Done.
	return(EXIT_SUCCESS);
}
