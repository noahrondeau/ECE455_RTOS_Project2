/*
 * ShiftReg.c
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#include "ShiftReg.h"

// Initialize the GPIOs that are necessary for the shift register operation
void ShiftReg_Init(void)
{
	//enable clocks for Shift register gpio port
	RCC_AHB1PeriphClockCmd(SHIFT_REG_CLOCK_SOURCE, ENABLE);

	// Initialize GPIO port C as push-pull, no pu-pd, output
	// Set speed to 2 MHz so that we can safely chain consecutive
	// set and reset calls without delay
	// (2MHz is much slower than the shift reg max speed of 50 MHz)
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
	//reset and set the clear pin to wipe the register to 0000...
	GPIO_ResetBits(SHIFT_REG_PORT, SHIFT_REG_CLEAR);
	GPIO_SetBits(SHIFT_REG_PORT, SHIFT_REG_CLEAR);
}

// Create a rising edge on the clock pin
void ShiftReg_ClkRise(void)
{
	GPIO_SetBits(SHIFT_REG_PORT, SHIFT_REG_CLK);
}

// Create a falling edge on the clock pin
void ShiftReg_ClkFall(void)
{
	GPIO_ResetBits(SHIFT_REG_PORT, SHIFT_REG_CLK);
}

// Shift ONLY the LSB of val onto the register
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
	// bit0: "newest"
	// bit21: "oldest"
	// bits 22 - 31: dont care
	// bits shifted in starting with LSB

	ShiftReg_Clear();

	// move LSB into register by inputting, pulsing the clock
	// then move next bit down to LSB before iterating again
	for (int i = 0; i < NUM_LED; i++)
	{
		ShiftReg_ShiftBitOnto(seq); // shift LSB into reg
		ShiftReg_ClkRise(); // clock up
		ShiftReg_ClkFall(); // clock down
		seq = seq >> 1; // prepare sequence for net iteration
	}
}
