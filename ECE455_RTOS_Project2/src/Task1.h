#ifndef Task1_h
#define Task1_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <stdint.h>
#include "stm32f4xx_adc.h"

#include "config.h"

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Task1___Init(ADC_TypeDef* which_ADC_using, uint32_t u32___ms_between_ADC_reads);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
