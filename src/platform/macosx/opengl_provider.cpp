#include <scythe/opengl/opengl_provider.h>

#include <scythe/opengl/opengl_include.h>
#include <scythe/desktop_application.h>
#include <scythe/log.h>

#include "nsgl_context.h"

/**
 * @brief      Gets the proc address.
 *
 * @param[in]  name  The name
 *
 * @return     The proc address.
 */
static GLADapiproc GetProcedureAddress(const char* name)
{
	scythe::LibraryNSGL* nsgl = scythe::LibraryNSGL::GetInstance();
	return nsgl->GetProcedureAddress(name);
}

namespace scythe {

	bool OpenGLGraphicsProvider::Initialize()
	{
		LibraryNSGL* nsgl = LibraryNSGL::GetInstance();
		// Initialize NSGL
		if (!nsgl->Initialize())
			return false;

		// Context configuration
		OpenGLContextConfiguration context_config;
		context_config.major = context_major_version_;
		context_config.minor = context_minor_version_;
		context_config.profile = OpenGLContextProfile::kCore;
		context_config.forward = true;
		context_config.noerror = false;
		context_config.debug = false;

		// Framebuffer configuration
		OpenGLFramebufferConfiguration fb_config;
		fb_config.red_bits = red_bits_;
		fb_config.green_bits = green_bits_;
		fb_config.blue_bits = blue_bits_;
		fb_config.alpha_bits = alpha_bits_;
		fb_config.depth_bits = depth_bits_;
		fb_config.stencil_bits = stencil_bits_;
		fb_config.accum_red_bits = 0;
		fb_config.accum_green_bits = 0;
		fb_config.accum_blue_bits = 0;
		fb_config.accum_alpha_bits = 0;
		fb_config.aux_buffers = 0;
		fb_config.samples = 0;
		fb_config.stereo = false;
		fb_config.sRGB = false;
		fb_config.doublebuffer = true;
		fb_config.transparent = false;

		// Create OpenGL context
		if (!nsgl->CreateContext(&context_config, &fb_config))
		{
			nsgl->Deinitialize();
			return false;
		}

		MakeContextCurrent();

		// Load GLAD
		if (!gladLoadGL(::GetProcedureAddress))
		{
			Error("GLAD loading failed");
			return false;
		}

		// Finally
		initialized_ = true;

		return true;
	}
	void OpenGLGraphicsProvider::Deinitialize()
	{
		LibraryNSGL* nsgl = LibraryNSGL::GetInstance();
		nsgl->DestroyContext();
		nsgl->Deinitialize();
	}
	void OpenGLGraphicsProvider::BeginFrame()
	{
		// Nothing to do here
	}
	void OpenGLGraphicsProvider::EndFrame()
	{
		LibraryNSGL* nsgl = LibraryNSGL::GetInstance();
		nsgl->SwapBuffers();
	}
	void OpenGLGraphicsProvider::SetSwapInterval(int interval)
	{
		LibraryNSGL* nsgl = LibraryNSGL::GetInstance();
		nsgl->SetSwapInterval(interval);
	}
	void OpenGLGraphicsProvider::MakeContextCurrent()
	{
		LibraryNSGL* nsgl = LibraryNSGL::GetInstance();
		nsgl->MakeContextCurrent();
	}

} // namespace scythe