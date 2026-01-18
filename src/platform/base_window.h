#ifndef __SCYTHE_BASE_WINDOW_H__
#define __SCYTHE_BASE_WINDOW_H__

namespace scythe {

	/**
	 * @brief      This structure describes base window parameters.
	 */
	struct BaseWindow
	{
		int width;				//!< width in pixels
		int height;				//!< height in pixels
		float aspect_ratio;		//!< width over height ratio
		bool active;			//!< is window active
		bool visible;			//!< is window visible
		bool fullscreen;		//!< is window fullscreen
		char pad[1];
	};

} // namespace scythe

#endif // __SCYTHE_BASE_WINDOW_H__