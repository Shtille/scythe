#ifndef __SCYTHE_WINDOW_H__
#define __SCYTHE_WINDOW_H__

namespace scythe {

	/**
	 * @brief      This structure describes window parameters.
	 */
	struct Window
	{
		int width;				//!< width in pixels
		int height;				//!< height in pixels
		float aspect_ratio;		//!< width over height ratio
		bool visible;			//!< is window visible
		bool fullscreen;		//!< is window fullscreen
		char pad[2];
	};

} // namespace scythe

#endif