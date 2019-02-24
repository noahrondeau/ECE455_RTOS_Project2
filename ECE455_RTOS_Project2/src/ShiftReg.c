/*
 * ShiftReg.c
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#include "ShiftReg.h"


void ShiftReg_Init(void)
{
	//enable clocks for Shift register gpio port
	RCC_AHB1PeriphClockCmd(SHIFT_REG_CLOCK_SOURCE, ENABLE);

	GPIO_InitTypeDef init;
	init.GPIO_Mode = GPIO_Mode_OUT;
	init.GPIO_OType = GPIO_OType_PP;
	init.GPIO_Pin =  SHIFT_REG_PIN | SHIFT_REG_CLK;
	init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	init.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SHIFT_REG_PORT, &init);
}
