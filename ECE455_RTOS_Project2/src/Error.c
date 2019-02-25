//==================================== interface ==========================================================================================
//  void Error(const char* const PZ___FUNCTION_SIGNATURE, const char* const PZ___FORMAT, ...);
//=========================================================================================================================================

//------------------------------------ include --------------------------------------------------------------------------------------------
#include "Error.h"

#include <stdarg.h>
#include <stdio.h>

//------------------------------------ functions ------------------------------------------------------------------------------------------
void Error(const char* const PZ___FUNCTION_SIGNATURE, const char* const PZ___FORMAT, ...)
{
	va_list args;
	va_start(args, PZ___FORMAT);
	
	printf("<error> %s; ", PZ___FUNCTION_SIGNATURE);
	vprintf(PZ___FORMAT, args);
	
	va_end(args);
}
