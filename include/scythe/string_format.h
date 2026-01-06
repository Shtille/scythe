#ifndef __SCYTHE_STRING_FORMAT_H__
#define __SCYTHE_STRING_FORMAT_H__

#include <string>
#include <cstring>

namespace scythe {

	/**
	 * Formats string like sprintf.
	 * Version with char*
	 */
	template <typename ... Args>
	std::string string_format(const char* format, Args ... args) noexcept(false)
	{
		std::size_t size = snprintf(nullptr, 0, format, args ...);
		std::string result(size, '\0');
		snprintf(&result[0], size + 1, format, args ...); // buffer should hold extra space for '\0'
		return result;
	}
	/**
	 * Formats string like sprintf.
	 * Version with std::string
	 */
	template <typename ... Args>
	std::string string_format(const std::string& format, Args ... args) noexcept(false)
	{
		std::size_t size = snprintf(nullptr, 0, format.c_str(), args ...);
		std::string result(size, '\0');
		snprintf(&result[0], size + 1, format.c_str(), args ...); // buffer should hold extra space for '\0'
		return result;
	}

} // namespace scythe

#endif