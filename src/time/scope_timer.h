#ifndef __SCYTHE_SCOPE_TIMER_H__
#define __SCYTHE_SCOPE_TIMER_H__

namespace scythe {
	
	// Forward declarations
	class Stream;
	class ScopeTimerImpl;

	//! Class for scope time measurement
	class ScopeTimer {
	public:
		ScopeTimer(Stream * stream, const char* format);
		~ScopeTimer();
		
	private:
		ScopeTimerImpl * impl_; //!< implementation
	};
	
} // namespace scythe

#endif