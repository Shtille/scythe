#ifndef __SCYTHE_OPENGL_CONTEXT_CONFIG_H__
#define __SCYTHE_OPENGL_CONTEXT_CONFIG_H__

#include <scythe/types.h>

namespace scythe {

	/**
	 * @brief      This class describes an OpenGL context profile.
	 */
	enum class OpenGLContextProfile : uint8_t
	{
		kDefault = 0,
		kCore = 1,
		kCompatibility = 2,
	};

	/**
	 * @brief      Declares OpenGL context configuration
	 */
	struct OpenGLContextConfiguration
	{
		int major;
		int minor;
		OpenGLContextProfile profile;
		bool forward;
		bool noerror;
		bool debug;
	};

} // namespace scythe

#endif // __SCYTHE_OPENGL_CONTEXT_CONFIG_H__