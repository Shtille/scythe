#ifndef __SCYTHE_NON_COPYABLE_H__
#define __SCYTHE_NON_COPYABLE_H__

namespace scythe {

	//! Non-copyable class interface
	class NonCopyable {
	public:
		NonCopyable() = default;
		virtual ~NonCopyable() = default;

	private:
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator =(const NonCopyable&) = delete;
	};

} // namespace scythe

#endif