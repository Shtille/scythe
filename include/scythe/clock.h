#ifndef __SCYTHE_CLOCK_H__
#define __SCYTHE_CLOCK_H__

#include <chrono>

namespace scythe {

	/**
	 * @brief      This class describes a clock.
	 */
	class Clock
	{
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<float> seconds;
		
	public:

		/**
		 * @brief      Constructs a new instance.
		 */
		Clock();

		/**
		 * @brief      Makes current time as a start point
		 */
		void MakeStartPoint(); //!< makes current time as a start point

		/**
		 * @brief      Gets difference between current time and start time in seconds
		 *
		 * @return     The time difference.
		 */
		float GetTime() const;

	private:
		std::chrono::time_point<clock> start_time_;
		mutable std::chrono::time_point<clock> end_time_;
	};

} // namespace scythe

#endif