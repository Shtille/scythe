#include "nsgl_context.h"

#include <scythe/desktop_application.h>
#include <scythe/log.h>

#include "platform_data.h"
#include "../platform_inner.h"

static inline scythe::platform::Window* GetMainWindow()
{
	scythe::Application* app = scythe::Application::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	return data->main_window;
}

namespace scythe {

	bool LibraryNSGL::Initialize()
	{
		if (framework_)
			return true;

		framework_ = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
		if (framework_ == nullptr)
		{
			scythe::Error("NSGL: Failed to locate OpenGL framework");
			return false;
		}

		return true;
	}
	void LibraryNSGL::Deinitialize()
	{
		//
	}
	bool LibraryNSGL::CreateContext(const OpenGLContextConfiguration* context_config, const OpenGLFramebufferConfiguration* fb_config)
	{
		platform::Window* window = ::GetMainWindow();

		if (context_config->major > 2)
		{
			if (context_config->major == 3 && context_config->minor < 2)
			{
				scythe::Error("NSGL: The targeted version of macOS does not support OpenGL 3.0 or 3.1 but may support 3.2 and above");
				return false;
			}
		}

		if (context_config->major >= 3 && context_config->profile == OpenGLContextProfile::kCompatibility)
		{
			scythe::Error("NSGL: The compatibility profile is not available on macOS");
			return false;
		}

		// Context robustness modes (GL_KHR_robustness) are not supported by
		// macOS but are not a hard constraint, so ignore and continue

		// Context release behaviors (GL_KHR_context_flush_control) are not 
		// supported by macOS but are not a hard constraint, so ignore and continue

		// Debug contexts (GL_KHR_debug) are not supported by macOS but are not
		// a hard constraint, so ignore and continue

		// No-error contexts (GL_KHR_no_error) are not supported by macOS but
		// are not a hard constraint, so ignore and continue

#define ADD_ATTRIB(a) \
{ \
	assert((size_t) index < sizeof(attribs) / sizeof(attribs[0])); \
	attribs[index++] = a; \
}
#define SET_ATTRIB(a, v) { ADD_ATTRIB(a); ADD_ATTRIB(v); }

		constexpr int kDontCare = -1;
		NSOpenGLPixelFormatAttribute attribs[40];
		int index = 0;

		ADD_ATTRIB(NSOpenGLPFAAccelerated);
		ADD_ATTRIB(NSOpenGLPFAClosestPolicy);

		if (context_config->nsgl.offline)
		{
			ADD_ATTRIB(NSOpenGLPFAAllowOfflineRenderers);
			// NOTE: This replaces the NSSupportsAutomaticGraphicsSwitching key in
			//       Info.plist for unbundled applications
			// HACK: This assumes that NSOpenGLPixelFormat will remain
			//       a straightforward wrapper of its CGL counterpart
			ADD_ATTRIB(kCGLPFASupportsAutomaticGraphicsSwitching);
		}

		if (context_config->major >= 4)
		{
			SET_ATTRIB(NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core);
		}
		else if (context_config->major >= 3)
		{
			SET_ATTRIB(NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core);
		}

		if (context_config->major <= 2)
		{
			if (fb_config->aux_buffers != kDontCare)
				SET_ATTRIB(NSOpenGLPFAAuxBuffers, fb_config->aux_buffers);

			if (fb_config->accum_red_bits != kDontCare &&
				fb_config->accum_green_bits != kDontCare &&
				fb_config->accum_blue_bits != kDontCare &&
				fb_config->accum_alpha_bits != kDontCare)
			{
				const int accum_bits = fb_config->accum_red_bits +
									   fb_config->accum_green_bits +
									   fb_config->accum_blue_bits +
									   fb_config->accum_alpha_bits;

				SET_ATTRIB(NSOpenGLPFAAccumSize, accum_bits);
			}
		}

		if (fb_config->red_bits != kDontCare &&
			fb_config->green_bits != kDontCare &&
			fb_config->blue_bits != kDontCare)
		{
			int color_bits = fb_config->red_bits +
							 fb_config->green_bits +
							 fb_config->blue_bits;

			// macOS needs non-zero color size, so set reasonable values
			if (color_bits == 0)
				color_bits = 24;
			else if (color_bits < 15)
				color_bits = 15;

			SET_ATTRIB(NSOpenGLPFAColorSize, color_bits);
		}

		if (fb_config->alpha_bits != kDontCare)
			SET_ATTRIB(NSOpenGLPFAAlphaSize, fb_config->alpha_bits);

		if (fb_config->depth_bits != kDontCare)
			SET_ATTRIB(NSOpenGLPFADepthSize, fb_config->depth_bits);

		if (fb_config->stencil_bits != kDontCare)
			SET_ATTRIB(NSOpenGLPFAStencilSize, fb_config->stencil_bits);

		if (fb_config->stereo)
		{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 101200
			scythe::Error("NSGL: Stereo rendering is deprecated");
			return false;
#else
			ADD_ATTRIB(NSOpenGLPFAStereo);
#endif
		}

		if (fb_config->doublebuffer)
			ADD_ATTRIB(NSOpenGLPFADoubleBuffer);

		if (fb_config->samples != kDontCare)
		{
			if (fb_config->samples == 0)
			{
				SET_ATTRIB(NSOpenGLPFASampleBuffers, 0);
			}
			else
			{
				SET_ATTRIB(NSOpenGLPFASampleBuffers, 1);
				SET_ATTRIB(NSOpenGLPFASamples, fb_config->samples);
			}
		}

		// NOTE: All NSOpenGLPixelFormats on the relevant cards support sRGB
		//       framebuffer, so there's no need (and no way) to request it

		ADD_ATTRIB(0);

#undef ADD_ATTRIB
#undef SET_ATTRIB

		window->context.pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
		if (window->context.pixel_format == nil)
		{
			scythe::Error("NSGL: Failed to find a suitable pixel format");
			return false;
		}

		NSOpenGLContext* share = nil;

		// if (context_config->share)
		// 	share = context_config->share->context.nsgl.object;

		window->context.object =
			[[NSOpenGLContext alloc] initWithFormat:window->context.pixel_format
									   shareContext:share];
		if (window->context.object == nil)
		{
			scythe::Error("NSGL: Failed to create OpenGL context");
			return false;
		}

		if (fb_config->transparent)
		{
			GLint opaque = 0;
			[window->context.object setValues:&opaque
									forParameter:NSOpenGLContextParameterSurfaceOpacity];
		}

		if (context_config->profile == OpenGLContextProfile::kCore)
		{
			// When we're using a CoreProfile context, crash if we call a legacy OpenGL function
			// This will make it much more obvious where and when such a function call is made so
			// that we can remove such calls.
			// Without this we'd simply get GL_INVALID_OPERATION error for calling legacy functions
			// but it would be more difficult to see where that function was called.
			CGLEnable([window->context.object CGLContextObj], kCGLCECrashOnRemovedFunctions);
		}

		//[window->view setWantsBestResolutionOpenGLSurface:window->scale_framebuffer];

		[window->context.object setView:window->view];

		return true;
	}
	void LibraryNSGL::DestroyContext()
	{
		platform::Window* window = ::GetMainWindow();

		[window->context.pixel_format release];
		window->context.pixel_format = nil;

		[window->context.object release];
		window->context.object = nil;
	}
	bool LibraryNSGL::IsExtensionSupported(const char* extension)
	{
		// NSGL doesnt support any extensions, OpenGL is deprecated on Mac OS X
		return false;
	}
	LibraryNSGL::Procedure LibraryNSGL::GetProcedureAddress(const char* name)
	{
		CFStringRef symbol_name = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
		Procedure symbol = (Procedure) CFBundleGetFunctionPointerForName(framework_, symbol_name);
		CFRelease(symbol_name);
		return symbol;
	}
	void LibraryNSGL::SwapBuffers()
	{
		platform::Window* window = ::GetMainWindow();

		[window->context flushBuffer];
	}
	void LibraryNSGL::MakeContextCurrent()
	{
		platform::Window* window = ::GetMainWindow();

		[window->context makeCurrentContext];
	}
	void LibraryNSGL::SetSwapInterval(int interval)
	{
		platform::Window* window = ::GetMainWindow();

		GLint swapInt = (GLint)interval;
	#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101400
		const NSOpenGLContextParameter kSwapInterval = NSOpenGLCPSwapInterval;
	#else
		const NSOpenGLContextParameter kSwapInterval = NSOpenGLContextParameterSwapInterval;
	#endif
		[window->context setValues:&swapInt forParameter:kSwapInterval];
	}

} // namespace scythe