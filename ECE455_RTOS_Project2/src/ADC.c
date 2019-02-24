/*
 * ADC.c
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#include "ADC.h"

void MyADC_Init()
{

	// enable clocks for ADC, GPIO
	RCC_AHB1PeriphClockCmd(POT_GPIO_CLOCK_SOURCE, ENABLE);
	RCC_APB2PeriphClockCmd(POT_ADC_CLOCK_SOURCE, ENABLE);

	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Mode = GPIO_Mode_AN;
	gpio_init.GPIO_Pin = GPIO_Pin_1;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(POT_PORT, &gpio_init);

	ADC_InitTypeDef adc_init;
	adc_init.ADC_Resolution = ADC_Resolution_12b;
	adc_init.ADC_ScanConvMode = DISABLE;
	adc_init.ADC_DataAlign = ADC_DataAlign_Right;
	adc_init.ADC_ExternalTrigConv = DISABLE;
	adc_init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	adc_init.ADC_NbrOfConversion = 1;
	ADC_Init(POT_ADC, &adc_init);
	ADC_Cmd(POT_ADC, ENABLE);

	// ADC regular channel config
	ADC_RegularChannelConfig(POT_ADC, POT_ADC_CHANNEL, 1, ADC_SampleTime_84Cycles);
}
