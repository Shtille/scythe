#ifndef __SCYTHE_NON_COPYABLE_H__
#define __SCYTHE_NON_COPYABLE_H__

namespace scythe {

	/**
	 * @brief      This class describes a non copyable.
	 */
	class NonCopyable
	{
	public:
		NonCopyable() = default;
		virtual ~NonCopyable() = default;

	private:
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable(NonCopyable&&) = delete;
		NonCopyable& operator =(const NonCopyable&) = delete;
		NonCopyable& operator =(NonCopyable&&) = delete;
	};

} // namespace scythe

#endif