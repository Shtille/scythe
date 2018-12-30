#ifndef __SCYTHE_STRING_FORMAT_H__
#define __SCYTHE_STRING_FORMAT_H__

#include <string>
#include <cstring>

namespace scythe {

	template <typename ... Args>
	std::string string_format(const std::string& format, Args ... args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // extra space for '\0'
		std::string result(size, '\0');
		snprintf(&result[0], size, format.c_str(), args ...);
		return result;
	}

} // namespace scythe

#endif