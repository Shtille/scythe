#include <scythe/opengl_provider.h>

#include <scythe/opengl_include.h>
#include <scythe/desktop_application.h>
#include <scythe/log.h>
#include "platform_data.h"
#include "../platform_inner.h"
#include "wgl.h"

/**
 * @brief      Gets the platform window.
 *
 * @param[in]  app   The application
 *
 * @return     The platform window.
 */
static inline scythe::platform::Window* GetMainWindow(scythe::Application* app)
{
	scythe::platform::Data* data = scythe::platform::GetData(app);
	return data->main_window;
}

namespace scythe {

	static GLADapiproc _GetProcAddress(const char* name)
	{
		LibraryWGL* wgl = LibraryWGL::GetInstance();
		return wgl->GetProcedureAddress(name);
	}

	bool OpenGLGraphicsProvider::Initialize()
	{
		LibraryWGL* wgl = LibraryWGL::GetInstance();
		// Initialize WGL
		if (!wgl->Initialize())
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
		if (!wgl->CreateContext(&context_config, &fb_config))
		{
			wgl->Deinitialize();
			return false;
		}

		MakeContextCurrent();

		// Load GLAD
		if (!gladLoadGL(_GetProcAddress))
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
		LibraryWGL* wgl = LibraryWGL::GetInstance();
		wgl->DestroyContext();
		wgl->Deinitialize();
	}
	void OpenGLGraphicsProvider::BeginFrame()
	{
		// Nothing to do here
	}
	void OpenGLGraphicsProvider::EndFrame()
	{
		// Swap buffers
		platform::Window* window = ::GetMainWindow(application_);

		::SwapBuffers(window->context.dc);
	}
	void OpenGLGraphicsProvider::SetSwapInterval(int interval)
	{
		LibraryWGL* wgl = LibraryWGL::GetInstance();

		if (wgl->EXT_swap_control)
			wgl->wglSwapIntervalEXT(interval);
	}
	void OpenGLGraphicsProvider::MakeContextCurrent()
	{
		LibraryWGL* wgl = LibraryWGL::GetInstance();
		platform::Window* window = ::GetMainWindow(application_);

		wgl->wglMakeCurrent(window->context.dc, window->context.handle);
	}

} // namespace scythe