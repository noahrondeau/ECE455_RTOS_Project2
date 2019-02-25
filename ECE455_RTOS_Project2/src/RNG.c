//==================================== interface ==========================================================================================
//  void      RNG___Init(void);
//  uint32_t  RNG___Read(void);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include <stdint.h>
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"

#include "defs.h"

//------------------------------------ functions ------------------------------------------------------------------------------------------
void RNG___Init(void)
{
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);
}

uint32_t RNG___Read(void)
{
START:
	// Wait for a random number to be available.	
	while( ! RNG_GetFlagStatus(RNG_FLAG_DRDY) );
	
	// Check if any errors have occurred.
	FlagStatus x___seed_error = RNG_GetFlagStatus(RNG_FLAG_SECS);
	FlagStatus x___clock_error = RNG_GetFlagStatus(RNG_FLAG_CECS);
	if (x___seed_error) Error(FUNCTION_SIGNATURE, "A RNG seed error has been detected.\n");
	if (x___clock_error) Error(FUNCTION_SIGNATURE, "A RNG clock error has been detected. Don't know what to do about this. RNG won't be functioning properly.\n");
	
	// Correct for any "seed error".
	if (x___seed_error)
	{
		// A seed error has occurred.
		// Need to reset the RNG and try again.
		
		// Clear the "seed error" flag
		RNG_ClearFlag(RNG_FLAG_SECS);
		
		// Reset the RNG.
		RNG_Cmd(DISABLE);
		RNG_Cmd(ENABLE);
		
		// Try again.
		goto START;
	}
	
	// Can't correct for a "clock error". Not sure what to do.
	
	// Read in the random number.
	return( RNG_GetRandomNumber() );
}
