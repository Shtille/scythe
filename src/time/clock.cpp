#include <scythe/clock.h>

namespace scythe {

	Clock::Clock()
	{
		end_time_ = start_time_ = clock::now();
	}
	void Clock::MakeStartPoint()
	{
		start_time_ = clock::now();
	}
	float Clock::GetTime() const
	{
		end_time_ = clock::now();
		return std::chrono::duration_cast<seconds>(end_time_ - start_time_).count();
	}

} // namespace scythe