#include "log.h"

#ifndef SC_LOG_TO_FILE

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

namespace scythe {

static bool Write(const void *buffer, size_t size)
{
	return fwrite(buffer, size, 1, stdout) == 1;
}

bool PrintLine(const char* string, ...)
{
	char		text[256];		// Holds Our String
	va_list		ap;				// Pointer To List Of Arguments

	va_start(ap, string);				// Parses The String For Variables
	vsprintf(text, string, ap);		    // And Converts Symbols To Actual Numbers
	va_end(ap);							// Results Are Stored In Text

	return Write(text, strlen(text)) &&
			Write("\n", sizeof(char));
}

} // namespace scythe

#endif // SC_LOG_TO_FILE