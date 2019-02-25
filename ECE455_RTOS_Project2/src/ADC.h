/*
 * ADC.h
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#ifndef ADC_H_
#define ADC_H_

#include "config.h"
#include <stdint.h>
#include "stm32f4xx_adc.h"
#include "defs.h"

#define ADC_VALUE_MAX 0b111111111111;

void MyADC_Init(void);
uint16_t ADC___Read(ADC_TypeDef* ADCx);

#endif /* ADC_H_ */
