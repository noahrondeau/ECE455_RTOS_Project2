/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


/* ECE 455 Lab Project 2
 * Authors:
 * 		Noah Rondeau
 * 		Matthew Wegener
 * 		Geoff Hale
 * Completed 2019-03-07
 *
 * This project implements a simple simulation of a traffic
 * light on a one way street. Traffic Flow is adjusted by reading a potentiometer.
 * The traffic light duration is dynamically adjusted for traffic flow.
 * The system state is displayed every second by lighting LEDs hooked up to a
 * shift register.
 */

/* Includes */
#include <MessageChannel.h>
#include "config.h" // includes all necessary headers, defines, etc
#include "ADC.h"
#include "ShiftReg.h"
#include "Task1___potentiometer_reader.h"
#include "Task2___traffic_creator.h"
#include "DisplayTask.h"
#include "TrafficLight.h"
#include "OneShot_Timer.h"

/* Global Variable Definitions */

TrafficLight_t trafficLight; // represents the global traffic light
OneShot_Timer trafficLightTimer; // timer used for the traffic light
SemaphoreHandle_t xLightMutex; // mutex to protect traffic light access
EventGroupHandle_t xEvent; // event group for signalling cars

// queues for traffic flow communication
// MessageChannel is a wrapper around a queue which handles
// the logic of queue access
MessageChannel  g___message_channel___flow_rate_1_2;
MessageChannel  g___message_channel___flow_rate_1_3;

/* Local Function Definitions */
static void prvSetupHardware( void );
// initialization functions
void vInitializeHardware( void );
void vInitializeGlobals( void );
void vInitializeTasks( void );


/*---------------------  MAIN FUNCTION ----------------------*/

int main(void)
{
	// Initialize all elements of system
	vInitializeHardware();
	vInitializeGlobals();
	vInitializeTasks();

	// start the scheduler and spin forever
	vTaskStartScheduler();
	return 0;
}


/*-----------------------------------------------------------*/

// Initializes the hardware and low-level drivers
void vInitializeHardware( void )
{
	// Initialize necessary GPIO and ADC pins
	ShiftReg_Init();
	MyADC_Init();
	// Init the random number generator
	RNG___Init();
	prvSetupHardware();
}

// Initializes the global variables with their heap-allocated FreeRTOS objects
void vInitializeGlobals( void )
{
	vTrafficLightInit(&trafficLight);
	xLightMutex = xSemaphoreCreateMutex();
	xEvent = xEventGroupCreate();

	// Initialize queues, but check for errors
	EXIT_STATUS exit_status;
	exit_status = MessageChannel___Create(&g___message_channel___flow_rate_1_2, sizeof(float));
	if (exit_status != 0)
	{
		// see error.h
		Error(FUNCTION_SIGNATURE, "Failed to init message channel FROM task1 TO task2.\n");
	}

	exit_status = MessageChannel___Create(&g___message_channel___flow_rate_1_3, sizeof(float));
	if (exit_status != 0)
	{
		//see error.h
		Error(FUNCTION_SIGNATURE, "Failed to init message channel FROM task1 TO task3.\n");
	}
}

// Initialize all the necessary tasks
void vInitializeTasks( void )
{
	// these two functions wrap some logic that needs to be done before creating these tasks
	Task1___potentiometer_reader___Init(ADC1, TIME_PERIOD); // ADC polling
	Task2___traffic_creator___Init(0.0, 5.0, 0.7); // Traffic Flow Creation

	// These are fairly straightforward raw tasks
	xTaskCreate( vTrafficLightControlTask, "TafficLightControlTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( vDisplayTask, "DisplayTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

