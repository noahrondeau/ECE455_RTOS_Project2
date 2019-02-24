/*
 * config.h
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"


// SHIFT REGISTER HOOKUP DEFS
#define SHIFT_REG_PORT			GPIOC
#define SHIFT_REG_CLOCK_SOURCE	RCC_AHB1Periph_GPIOC
#define SHIFT_REG_PIN			GPIO_Pin_7
#define SHIFT_REG_CLK			GPIO_Pin_9

// POTENTIOMETER HOOKUP DEFS
#define POT_ADC					ADC1
#define POT_ADC_CHANNEL			ADC_Channel_1
#define POT_ADC_CLOCK_SOURCE	RCC_APB2Periph_ADC1
#define POT_GPIO_CLOCK_SOURCE	RCC_AHB1Periph_GPIOA
#define POT_PORT				GPIOA
#define POT_PIN					GPIO_Pin_1

#endif /* CONFIG_H_ */
