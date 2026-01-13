#ifndef __SCYTHE_LOG_H__
#define __SCYTHE_LOG_H__

namespace scythe {

	/**
	 * @brief          Outputs error message.
	 *
	 * @param[in]      format     The printf-like formatted message
	 * @param[in,opt]  <unnamed>  Additional arguments
	 */
	void Error(const char* format, ...) noexcept;

} // namespace scythe

#endif // __SCYTHE_LOG_H__