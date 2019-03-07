/*
 * ShiftReg.h
 *
 *  Created on: Feb 24, 2019
 *      Author: ntron
 */

#ifndef SHIFTREG_H_
#define SHIFTREG_H_

#include "config.h"

/* This file defines the interface of the shift register
 * All pin definitions are located in config.h
 */

void ShiftReg_Init(void);

void ShiftReg_Clear(void);

void ShiftReg_ClkRise(void);

void ShiftReg_ClkFall(void);

// @param val: only the LSB gets shifted on
void ShiftReg_ShiftBitOnto(uint32_t val);

void ShiftReg_Update(uint32_t seq);

#endif /* SHIFTREG_H_ */
