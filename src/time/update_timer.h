#ifndef __SCYTHE_TIME_H__
#define __SCYTHE_TIME_H__

namespace scythe {

	//! Class for iteration time measurement
	class UpdateTimer {
	public:
		UpdateTimer();
		~UpdateTimer();

		void Start();
		float GetElapsedTime();

	private:
		class UpdateTimerImpl * impl_;
	};

} // namespace scythe

#endif