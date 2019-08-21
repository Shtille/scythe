#ifndef __SCYTHE_LOG_H__
#define __SCYTHE_LOG_H__

/**
 * Module provides defines for easy logging.
 */

#ifndef SC_LOG_TO_FILE

namespace scythe {

	/**
	 * Prints a line to standart output
     *
	 * @param string Formatted string
	 * @see printf
	 */
	bool PrintLine(const char* string, ...);

} // namespace scythe

# define SC_INFO scythe::PrintLine
# define SC_ERROR scythe::PrintLine

#else

# include "../stream/log_stream.h"

# define SC_INFO scythe::CommonLogStream::GetInstance()->PrintLine
# define SC_ERROR scythe::ErrorLogStream::GetInstance()->PrintLine

#endif // SC_LOG_TO_FILE

#endif