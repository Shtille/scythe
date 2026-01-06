#ifndef __SCYTHE_TIMER_H__
#define __SCYTHE_TIMER_H__

namespace scythe {

	/**
	 * @brief      This class describes a timer.
	 * @details    The timer is managed by @ref TimeManager.
	 *             Initially disabled.
	 * @see        TimeManager
	 */
	class Timer
	{
		friend class TimeManager;

	public:
		/**
		 * @brief      Resets the timer (sets time to 0).
		 */
		void Reset();

		/**
		 * @brief      Starts the timer.
		 */
		void Start();

		/**
		 * @brief      Stops the timer.
		 */
		void Stop();

		/**
		 * @brief      Determines if timer expired.
		 *
		 * @return     True if expired, False otherwise.
		 */
		bool IsExpired() const;

		/**
		 * @brief      Determines if timer enabled.
		 *
		 * @return     True if enabled, False otherwise.
		 */
		bool IsEnabled() const;

		/**
		 * @brief      Gets the timer interval.
		 *
		 * @return     The interval in seconds.
		 */
		float GetInterval() const;

		/**
		 * @brief      Gets the time elapsed from start.
		 *
		 * @return     The elapsed time in seconds.
		 */
		float GetTime() const;

	private:
		Timer(float interval);
		~Timer();

		Timer * next_;
		float interval_;
		float time_;
		bool enabled_;
	};

} // namespace scythe

#endif