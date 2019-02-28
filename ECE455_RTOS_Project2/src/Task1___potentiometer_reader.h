#ifndef Task1___potentiometer_reader_h
#define Task1___potentiometer_reader_h

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <stdint.h>
#include "stm32f4xx_adc.h"

#include "config.h"

//------------------------------------ functions ------------------------------------------------------------------------------------------
EXIT_STATUS Task1___potentiometer_reader___Init(ADC_TypeDef* which_ADC_using, uint32_t u32___ms_between_ADC_reads);

//------------------------------------ EOF ------------------------------------------------------------------------------------------------
#endif
