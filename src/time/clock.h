#ifndef __SCYTHE_CLOCK_H__
#define __SCYTHE_CLOCK_H__

namespace scythe {

	class ClockImpl;

	class Clock {
	public:
		Clock();
		~Clock();

		void MakeStartPoint(); //!< makes current time as a start point
		float GetTime() const; //!< returns difference between current time and start time in seconds

	private:
		ClockImpl * impl_;
	};

} // namespace scythe

#endif