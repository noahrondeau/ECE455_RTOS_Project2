/*
 * config.h
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

/* ExPLANATION:
 * This file includes anything that must be shared across
 * the whole system. Includes, definitions, configurations
 * i.e. Pin definitions, timing definitions, etc
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
#include "../FreeRTOS_Source/include/event_groups.h"

#include "Error.h"
#include "RNG.h"

#define FUNCTION_SIGNATURE  __PRETTY_FUNCTION__

#define intern              static
#define EXIT_STATUS         uint8_t


// SHIFT REGISTER HOOKUP DEFS
#define SHIFT_REG_PORT			GPIOC
#define SHIFT_REG_CLOCK_SOURCE	RCC_AHB1Periph_GPIOC
#define SHIFT_REG_INPUT			GPIO_Pin_7
#define SHIFT_REG_CLK			GPIO_Pin_8
#define SHIFT_REG_CLEAR			GPIO_Pin_9
#define NUM_LED					(22)

// POTENTIOMETER HOOKUP DEFS
#define POT_ADC					ADC1
#define POT_ADC_CHANNEL			ADC_Channel_1
#define POT_ADC_CLOCK_SOURCE	RCC_APB2Periph_ADC1
#define POT_GPIO_CLOCK_SOURCE	RCC_AHB1Periph_GPIOA
#define POT_PORT				GPIOA
#define POT_PIN					GPIO_Pin_1


#define ADC_VALUE_MAX (0b111111111111);

// Traffic length configs
#define ONCOMING_SHIFT	(0)
#define INTERSECT_SHIFT	(8)
#define LIGHT_SHIFT		(11)
#define OUTGOING_SHIFT	(14)

// Timing Configs
#define TIME_PERIOD		(1000)


#endif /* CONFIG_H_ */
