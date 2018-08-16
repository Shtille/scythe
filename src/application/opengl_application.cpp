#include "opengl_application.h"

#include "graphics/opengl/opengl_renderer.h"
#include "platform/window_controller.h"

namespace scythe {

	OpenGlApplication::OpenGlApplication()
	{

	}
	OpenGlApplication::~OpenGlApplication()
	{

	}
	bool OpenGlApplication::InitApi()
	{
		if (!PlatformInitOpenGLContext(color_bits_, depth_bits_, stencil_bits_))
			return false;

		PlatformMakeContextCurrent();

		if (IsBenchmark())
			PlatformSwapInterval(0);

		renderer_ = new OpenGlRenderer(width_, height_);

		return true;
	}
	void OpenGlApplication::DeinitApi()
	{
		renderer_->CleanUp();
		delete renderer_;

		PlatformDeinitOpenGLContext();
	}
	void OpenGlApplication::BeginFrame()
	{
		renderer_->Defaults();
	}
	void OpenGlApplication::EndFrame()
	{
		PlatformSwapBuffers();
	}

} // namespace scythe