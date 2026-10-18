#ifndef __SCYTHE_NSGL_H__
#define __SCYTHE_NSGL_H__

#include <scythe/singleton.h>
#include <scythe/platform_includes.h>

#include "../../graphics/opengl/opengl_context_config.h"
#include "../../graphics/opengl/opengl_framebuffer_config.h"

namespace scythe {

	/**
	 * @brief      This class describes a library WGL.
	 */
	class LibraryNSGL final
	: public Singleton<LibraryNSGL>
	{
	public:
		bool Initialize();
		void Deinitialize();

		bool CreateContext(const OpenGLContextConfiguration* context_config, const OpenGLFramebufferConfiguration* fb_config);
		void DestroyContext();

		bool IsExtensionSupported(const char* extension);

		typedef void (*Procedure)(void);
		Procedure GetProcedureAddress(const char* name);

		void SwapBuffers();
		void MakeContextCurrent();
		void SetSwapInterval(int interval);

	private:
#if defined(__OBJC__)
		CFBundleRef framework_;
#else
		void* framework_;
#endif
	};

} // namespace scythe

#endif