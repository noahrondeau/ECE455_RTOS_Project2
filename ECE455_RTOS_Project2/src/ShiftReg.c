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
	init.GPIO_Pin =  SHIFT_REG_INPUT | SHIFT_REG_CLK | SHIFT_REG_CLEAR;
	init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	init.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SHIFT_REG_PORT, &init);

	// initial clear of the register
	GPIO_ResetBits(SHIFT_REG_PORT, SHIFT_REG_CLEAR);
	GPIO_ResetBits(SHIFT_REG_PORT, SHIFT_REG_CLK);
	GPIO_SetBits(SHIFT_REG_PORT, SHIFT_REG_CLEAR);

}

void ShiftReg_Clear(void)
{
	GPIO_ResetBits(SHIFT_REG_PORT, SHIFT_REG_CLEAR);
	GPIO_SetBits(SHIFT_REG_PORT, SHIFT_REG_CLEAR);
}

void ShiftReg_ClkRise(void)
{
	GPIO_SetBits(SHIFT_REG_PORT, SHIFT_REG_CLK);
}

void ShiftReg_ClkFall(void)
{
	GPIO_ResetBits(SHIFT_REG_PORT, SHIFT_REG_CLK);
}

// @param val: only the LSB gets shifted on
void ShiftReg_ShiftBitOnto(uint32_t val)
{
	if ( (val & 0x00000001) == 1)
		GPIO_SetBits(SHIFT_REG_PORT, SHIFT_REG_INPUT);
	else
		GPIO_ResetBits(SHIFT_REG_PORT, SHIFT_REG_INPUT);
}

void ShiftReg_Update(uint32_t seq)
{
	// seq contains the LED sequence in the correct order:
	// bit0: "oldest" LED (first car to go through and that is still on the board)
	// bit21: "newest" LED (latest car to arrive in the queue)
	// bits 22 - 31: dont care

	ShiftReg_Clear();

	for (int i = 0; i < NUM_LED; i++)
	{
		ShiftReg_ShiftBitOnto(seq);
		ShiftReg_ClkRise();
		ShiftReg_ClkFall();
		seq = seq >> 1;
	}
}
