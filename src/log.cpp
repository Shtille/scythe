#include <scythe/log.h>

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * @brief      Outputs error message
 *
 * @param[in]  message  The message
 */
static void OutputErrorMessage(const char* message) noexcept
{
	fprintf(stderr, message);
}

namespace scythe {

	void Error(const char* format, ...) noexcept
	{
		va_list ap;
		va_start(ap, format);

		int count = vsnprintf(NULL, 0, format, ap);
		if (count < 1)
		{
			va_end(ap);
			return;
		}
		size_t size = static_cast<size_t>(count);
		char* result = new char[size+2]; // one spot for '\n' and one for '\0'
		if (!result)
			return;
		vsnprintf(&result[0], size+1, format, ap);
		result[size] = '\0';
		if (result[size-1] != '\n')
		{
			// Append new line character
			result[size] = '\n';
			result[size+1] = '\0';
		}

		va_end(ap);

		// Output
		::OutputErrorMessage(result);

		// Finally
		delete[] result;
	}

} // namespace scythe