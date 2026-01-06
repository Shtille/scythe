#include <scythe/timer.h>

namespace scythe {

	void Timer::Reset()
	{
		time_ = 0.0f;
	}
	void Timer::Start()
	{
		enabled_ = true;
	}
	void Timer::Stop()
	{
		enabled_ = false;
	}
	bool Timer::IsExpired() const
	{
		return time_ >= interval_;
	}
	bool Timer::IsEnabled() const
	{
		return enabled_;
	}
	float Timer::GetInterval() const
	{
		return interval_;
	}
	float Timer::GetTime() const
	{
		return time_;
	}
	Timer::Timer(float interval)
	: next_(nullptr)
	, interval_(interval)
	, time_(0.0f)
	, enabled_(false)
	{
	}
	Timer::~Timer()
	{
	}

} // namespace scythe