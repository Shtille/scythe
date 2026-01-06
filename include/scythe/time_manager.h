#ifndef __SCYTHE_TIME_MANAGER_H__
#define __SCYTHE_TIME_MANAGER_H__

#include "singleton.h"
#include "clock.h"
#include "timer.h"

namespace scythe {

	/**
	 * @brief      This class describes a time manager.
	 * @details    Creates and manages all @ref Timer class instances. 
	 *             Also calculates frame rate (FPS).
	 * @see        Timer
	 */
	class TimeManager final
	: private ManagedSingleton<TimeManager>
	{
		friend class ManagedSingleton<TimeManager>;
		friend class Application;

	public:

		/**
		 * Make GetInstance be accessible.
		 */
		using ManagedSingleton<TimeManager>::GetInstance;

		/**
		 * @brief      Gets the clock.
		 *
		 * @return     The clock.
		 */
		const Clock& GetClock() const;

		/**
		 * @brief      Adds a timer.
		 *
		 * @param[in]  interval  The timer interval in seconds
		 *
		 * @return     The created timer instance.
		 */
		Timer * AddTimer(float interval);

		/**
		 * @brief      Removes a previously created timer.
		 *
		 * @param      removed_timer  The removed timer
		 */
		void RemoveTimer(Timer * removed_timer);

		/**
		 * @brief      Gets the current time.
		 *
		 * @return     The current time in seconds.
		 */
		float GetTime() const;

		/**
		 * @brief      Gets the real frame time.
		 * @details    "Real" means this frame time is not a fixed value.
		 *
		 * @return     The frame time in seconds.
		 */
		float GetFrameTime() const;

		/**
		 * @brief      Gets the fixed frame time for updates.
		 *
		 * @return     The fixed frame time in seconds.
		 */
		float GetFixedFrameTime() const;

		/**
		 * @brief      Gets the frame rate (also known as FPS).
		 *
		 * @return     The frame rate.
		 */
		float GetFrameRate() const;

	private:
		TimeManager();
		~TimeManager();

		void SetFixedFrameTime(float fixed_frame_time);
		void Update();

		Clock clock_;
		Timer * timer_head_;
		float fixed_frame_time_;		//!< our engine uses fixed time steps, so this just shares the value
		float last_time_;
		float frame_time_;				//!< time between two updates
		float frame_rate_;				//!< frames per second (FPS)
		float fps_counter_time_;		//!< for counting FPS
		float fps_counter_count_;		//!< for counting FPS
	};

} // namespace scythe

#endif