#include "wgl.h"

#include <cstring>
#include <cassert>

#include <scythe/application.h>
#include <scythe/log.h>
#include "platform_data.h"
#include "../platform_inner.h"

#define ERROR_INVALID_VERSION_ARB 0x2095
#define ERROR_INVALID_PROFILE_ARB 0x2096
#define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054

/**
 * @brief      Determines if string in extension string.
 *
 * @param[in]  string      The string
 * @param[in]  extensions  The extensions
 *
 * @return     True if string in extension string, False otherwise.
 */
static bool IsStringInExtensionString(const char* string, const char* extensions)
{
	const char* start = extensions;

	for (;;)
	{
		const char* where;
		const char* terminator;

		where = std::strstr(start, string);
		if (!where)
			return false;

		terminator = where + std::strlen(string);
		if (where == start || *(where - 1) == ' ')
		{
			if (*terminator == ' ' || *terminator == '\0')
				break;
		}

		start = terminator;
	}

	return true;
}

/**
 * @brief      Finds the value corresponding to the specified attribute.
 *
 * @param[in]  attribs       The attributes
 * @param[in]  attrib_count  The attribute count
 * @param[in]  values        The values
 * @param[in]  attrib        The attribute
 *
 * @return     The found value.
 */
static int FindPixelFormatAttribValue(const int* attribs, int attrib_count, const int* values, int attrib)
{
	for (int i = 0; i < attrib_count; ++i)
	{
		if (attribs[i] == attrib)
			return values[i];
	}

	scythe::Error("WGL: Unknown pixel format attribute requested");
	return 0;
}

/**
 * @brief      Chooses the framebuffer config that best matches the desired one
 *
 * @param[in]  desired       The desired config
 * @param[in]  alternatives  The alternatives config array
 * @param[in]  count         The count
 *
 * @return     The best matched config.
 */
static const scythe::OpenGLFramebufferConfiguration* ChooseFramebufferConfig(
	   const scythe::OpenGLFramebufferConfiguration* desired, 
	   const scythe::OpenGLFramebufferConfiguration* alternatives, 
	   unsigned int count)
{
	constexpr int kDontCare = -1;
	constexpr unsigned int kUintMax = ~0;
	unsigned int i;
	unsigned int missing, least_missing = kUintMax;
	unsigned int color_diff, least_color_diff = kUintMax;
	unsigned int extra_diff, least_extra_diff = kUintMax;
	const scythe::OpenGLFramebufferConfiguration* current;
	const scythe::OpenGLFramebufferConfiguration* closest = nullptr;

	for (i = 0; i < count; ++i)
	{
		current = alternatives + i;

		if (desired->stereo && !current->stereo)
		{
			// Stereo is a hard constraint
			continue;
		}

		// Count number of missing buffers
		{
			missing = 0;

			if (desired->alpha_bits > 0 && current->alpha_bits == 0)
				missing++;

			if (desired->depth_bits > 0 && current->depth_bits == 0)
				missing++;

			if (desired->stencil_bits > 0 && current->stencil_bits == 0)
				missing++;

			if (desired->aux_buffers > 0 &&
				current->aux_buffers < desired->aux_buffers)
			{
				missing += desired->aux_buffers - current->aux_buffers;
			}

			if (desired->samples > 0 && current->samples == 0)
			{
				// Technically, several multisampling buffers could be
				// involved, but that's a lower level implementation detail and
				// not important to us here, so we count them as one
				missing++;
			}

			if (desired->transparent != current->transparent)
				missing++;
		}

		// These polynomials make many small channel size differences matter
		// less than one large channel size difference

		// Calculate color channel size difference value
		{
			color_diff = 0;

			if (desired->red_bits != kDontCare)
			{
				color_diff += (desired->red_bits - current->red_bits) *
							  (desired->red_bits - current->red_bits);
			}

			if (desired->green_bits != kDontCare)
			{
				color_diff += (desired->green_bits - current->green_bits) *
							  (desired->green_bits - current->green_bits);
			}

			if (desired->blue_bits != kDontCare)
			{
				color_diff += (desired->blue_bits - current->blue_bits) *
							  (desired->blue_bits - current->blue_bits);
			}
		}

		// Calculate non-color channel size difference value
		{
			extra_diff = 0;

			if (desired->alpha_bits != kDontCare)
			{
				extra_diff += (desired->alpha_bits - current->alpha_bits) *
							  (desired->alpha_bits - current->alpha_bits);
			}

			if (desired->depth_bits != kDontCare)
			{
				extra_diff += (desired->depth_bits - current->depth_bits) *
							  (desired->depth_bits - current->depth_bits);
			}

			if (desired->stencil_bits != kDontCare)
			{
				extra_diff += (desired->stencil_bits - current->stencil_bits) *
							  (desired->stencil_bits - current->stencil_bits);
			}

			if (desired->accum_red_bits != kDontCare)
			{
				extra_diff += (desired->accum_red_bits - current->accum_red_bits) *
							  (desired->accum_red_bits - current->accum_red_bits);
			}

			if (desired->accum_green_bits != kDontCare)
			{
				extra_diff += (desired->accum_green_bits - current->accum_green_bits) *
							  (desired->accum_green_bits - current->accum_green_bits);
			}

			if (desired->accum_blue_bits != kDontCare)
			{
				extra_diff += (desired->accum_blue_bits - current->accum_blue_bits) *
							  (desired->accum_blue_bits - current->accum_blue_bits);
			}

			if (desired->accum_alpha_bits != kDontCare)
			{
				extra_diff += (desired->accum_alpha_bits - current->accum_alpha_bits) *
							  (desired->accum_alpha_bits - current->accum_alpha_bits);
			}

			if (desired->samples != kDontCare)
			{
				extra_diff += (desired->samples - current->samples) *
							  (desired->samples - current->samples);
			}

			if (desired->sRGB && !current->sRGB)
				extra_diff++;
		}

		// Figure out if the current one is better than the best one found so far
		// Least number of missing buffers is the most important heuristic,
		// then color buffer size match and lastly size match for other buffers

		if (missing < least_missing)
			closest = current;
		else if (missing == least_missing)
		{
			if ((color_diff < least_color_diff) ||
				(color_diff == least_color_diff && extra_diff < least_extra_diff))
			{
				closest = current;
			}
		}

		if (current == closest)
		{
			least_missing = missing;
			least_color_diff = color_diff;
			least_extra_diff = extra_diff;
		}
	}

	return closest;
}

static inline scythe::platform::Window* GetMainWindow()
{
	scythe::Application* app = scythe::Application::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	return data->main_window;
}

namespace scythe {

	bool LibraryWGL::Initialize()
	{
		PIXELFORMATDESCRIPTOR pfd;
		HGLRC prc, rc;
		HDC pdc, dc;

		if (library_instance_)
			return true;

		library_instance_ = ::LoadLibraryA("opengl32.dll");
		if (!library_instance_)
		{
			Error("WGL: Failed to load opengl32.dll");
			return false;
		}

		HMODULE library_module = (HMODULE) library_instance_;

		wglCreateContext     = (PFN_wglCreateContext)     ::GetProcAddress(library_module, "wglCreateContext");
		wglDeleteContext     = (PFN_wglDeleteContext)     ::GetProcAddress(library_module, "wglDeleteContext");
		wglGetProcAddress    = (PFN_wglGetProcAddress)    ::GetProcAddress(library_module, "wglGetProcAddress");
		wglGetCurrentDC      = (PFN_wglGetCurrentDC)      ::GetProcAddress(library_module, "wglGetCurrentDC");
		wglGetCurrentContext = (PFN_wglGetCurrentContext) ::GetProcAddress(library_module, "wglGetCurrentContext");
		wglMakeCurrent       = (PFN_wglMakeCurrent)       ::GetProcAddress(library_module, "wglMakeCurrent");
		wglShareLists        = (PFN_wglShareLists)        ::GetProcAddress(library_module, "wglShareLists");

		// NOTE: A dummy context has to be created for opengl32.dll to load the
		//       OpenGL ICD, from which we can then query WGL extensions
		// NOTE: This code will accept the Microsoft GDI ICD; accelerated context
		//       creation failure occurs during manual pixel format enumeration

		Application* app = Application::GetInstance();
		platform::Data* data = platform::GetData(app);
		platform::Window* window = data->main_window;

		dc = ::GetDC(data->helper_window_handle);

		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;

		if (!::SetPixelFormat(dc, ::ChoosePixelFormat(dc, &pfd), &pfd))
		{
			Error("WGL: Failed to set pixel format for dummy context");
			return false;
		}

		rc = wglCreateContext(dc);
		if (!rc)
		{
			Error("WGL: Failed to create dummy context");
			return false;
		}

		pdc = wglGetCurrentDC();
		prc = wglGetCurrentContext();

		if (!wglMakeCurrent(dc, rc))
		{
			Error("WGL: Failed to make dummy context current");
			wglMakeCurrent(pdc, prc);
			wglDeleteContext(rc);
			return false;
		}

		// NOTE: Functions must be loaded first as they're needed to retrieve the
		//       extension string that tells us whether the functions are supported
		wglGetExtensionsStringEXT    = (PFN_wglGetExtensionsStringEXT)    wglGetProcAddress("wglGetExtensionsStringEXT");
		wglGetExtensionsStringARB    = (PFN_wglGetExtensionsStringARB)    wglGetProcAddress("wglGetExtensionsStringARB");
		wglCreateContextAttribsARB   = (PFN_wglCreateContextAttribsARB)   wglGetProcAddress("wglCreateContextAttribsARB");
		wglSwapIntervalEXT           = (PFN_wglSwapIntervalEXT)           wglGetProcAddress("wglSwapIntervalEXT");
		wglGetPixelFormatAttribivARB = (PFN_wglGetPixelFormatAttribivARB) wglGetProcAddress("wglGetPixelFormatAttribivARB");

		// NOTE: WGL_ARB_extensions_string and WGL_EXT_extensions_string are not
		//       checked below as we are already using them
		ARB_multisample                = IsExtensionSupported("WGL_ARB_multisample");
		ARB_framebuffer_sRGB           = IsExtensionSupported("WGL_ARB_framebuffer_sRGB");
		EXT_framebuffer_sRGB           = IsExtensionSupported("WGL_EXT_framebuffer_sRGB");
		ARB_create_context             = IsExtensionSupported("WGL_ARB_create_context");
		ARB_create_context_profile     = IsExtensionSupported("WGL_ARB_create_context_profile");
		EXT_create_context_es2_profile = IsExtensionSupported("WGL_EXT_create_context_es2_profile");
		ARB_create_context_robustness  = IsExtensionSupported("WGL_ARB_create_context_robustness");
		ARB_create_context_no_error    = IsExtensionSupported("WGL_ARB_create_context_no_error");
		EXT_swap_control               = IsExtensionSupported("WGL_EXT_swap_control");
		EXT_colorspace                 = IsExtensionSupported("WGL_EXT_colorspace");
		ARB_pixel_format               = IsExtensionSupported("WGL_ARB_pixel_format");
		ARB_context_flush_control      = IsExtensionSupported("WGL_ARB_context_flush_control");

		wglMakeCurrent(pdc, prc);
		wglDeleteContext(rc);
		return true;
	}
	void LibraryWGL::Deinitialize()
	{
		if (library_instance_)
		{
			::FreeLibrary((HMODULE) library_instance_);
		}
	}

#define SET_ATTRIB(a, v) \
{ \
	assert(((size_t) index + 1) < sizeof(attribs) / sizeof(attribs[0])); \
	attribs[index++] = a; \
	attribs[index++] = v; \
}

	bool LibraryWGL::CreateContext(const OpenGLContextConfiguration* context_config, const OpenGLFramebufferConfiguration* fb_config)
	{
		int attribs[40];
		int pixel_format;
		PIXELFORMATDESCRIPTOR pfd;
		HGLRC share = NULL;

		// We will not use shared context
		// if (ctxconfig->share)
		// 	share = ctxconfig->share->context.wgl.handle;

		if (context_config->forward)
		{
			if (!ARB_create_context)
			{
				Error("WGL: A forward compatible OpenGL context requested but WGL_ARB_create_context is unavailable");
				return false;
			}
		}
		if (context_config->profile != OpenGLContextProfile::kDefault)
		{
			if (!ARB_create_context_profile)
			{
				Error("WGL: OpenGL profile requested but WGL_ARB_create_context_profile is unavailable");
				return false;
			}
		}

		platform::Window* window = ::GetMainWindow();

		window->context.dc = ::GetDC(window->handle);
		if (!window->context.dc)
		{
			Error("WGL: Failed to retrieve DC for window");
			return false;
		}

		pixel_format = _ChoosePixelFormat(context_config, fb_config);
		if (!pixel_format)
			return false;

		if (!::DescribePixelFormat(window->context.dc, pixel_format, sizeof(pfd), &pfd))
		{
			Error("WGL: Failed to retrieve PFD for selected pixel format");
			return false;
		}

		if (!::SetPixelFormat(window->context.dc, pixel_format, &pfd))
		{
			Error("WGL: Failed to set selected pixel format");
			return false;
		}

		if (ARB_create_context)
		{
			int index = 0, mask = 0, flags = 0;

			if (context_config->forward)
				flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

			if (context_config->profile == OpenGLContextProfile::kCore)
				mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
			else if (context_config->profile == OpenGLContextProfile::kCompatibility)
				mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

			if (context_config->debug)
				flags |= WGL_CONTEXT_DEBUG_BIT_ARB;

			if (context_config->noerror)
			{
				if (ARB_create_context_no_error)
					SET_ATTRIB(WGL_CONTEXT_OPENGL_NO_ERROR_ARB, 1);
			}

			// NOTE: Only request an explicitly versioned context when necessary, as
			//       explicitly requesting version 1.0 does not always return the
			//       highest version supported by the driver
			if (context_config->major != 1 || context_config->minor != 0)
			{
				SET_ATTRIB(WGL_CONTEXT_MAJOR_VERSION_ARB, context_config->major);
				SET_ATTRIB(WGL_CONTEXT_MINOR_VERSION_ARB, context_config->minor);
			}

			if (flags)
				SET_ATTRIB(WGL_CONTEXT_FLAGS_ARB, flags);

			if (mask)
				SET_ATTRIB(WGL_CONTEXT_PROFILE_MASK_ARB, mask);

			SET_ATTRIB(0, 0);

			window->context.handle = wglCreateContextAttribsARB(window->context.dc, share, attribs);
			if (!window->context.handle)
			{
				const DWORD error = ::GetLastError();

				if (error == (0xc0070000 | ERROR_INVALID_VERSION_ARB))
				{
					Error("WGL: Driver does not support OpenGL version %i.%i", context_config->major, context_config->minor);
				}
				else if (error == (0xc0070000 | ERROR_INVALID_PROFILE_ARB))
				{
					Error("WGL: Driver does not support the requested OpenGL profile");
				}
				else if (error == (0xc0070000 | ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB))
				{
					Error("WGL: The share context is not compatible with the requested context");
				}
				else
				{
					Error("WGL: Failed to create OpenGL context");
				}

				return false;
			}
		}
		else
		{
			window->context.handle = wglCreateContext(window->context.dc);
			if (!window->context.handle)
			{
				Error("WGL: Failed to create OpenGL context");
				return false;
			}

			if (share)
			{
				if (!wglShareLists(share, window->context.handle))
				{
					Error("WGL: Failed to enable sharing with specified OpenGL context");
					return false;
				}
			}
		}

		return true;
	}

#undef SET_ATTRIB

	void LibraryWGL::DestroyContext()
	{
		platform::Window* window = ::GetMainWindow();

		if (window->context.handle)
		{
			wglDeleteContext(window->context.handle);
			window->context.handle = NULL;
		}
	}
	bool LibraryWGL::IsExtensionSupported(const char* extension)
	{
		const char* extensions = nullptr;

		if (wglGetExtensionsStringARB)
			extensions = wglGetExtensionsStringARB(wglGetCurrentDC());
		else if (wglGetExtensionsStringEXT)
			extensions = wglGetExtensionsStringEXT();

		if (!extensions)
			return false;

		return ::IsStringInExtensionString(extension, extensions);
	}
	LibraryWGL::Procedure LibraryWGL::GetProcedureAddress(const char* name)
	{
		const Procedure proc = (Procedure) wglGetProcAddress(name);
		if (proc)
			return proc;

		return (Procedure) ::GetProcAddress((HMODULE) library_instance_, name);
	}

#define ADD_ATTRIB(a) \
{ \
	assert((size_t) attrib_count < sizeof(attribs) / sizeof(attribs[0])); \
	attribs[attrib_count++] = a; \
}
#define FIND_ATTRIB_VALUE(a) \
	::FindPixelFormatAttribValue(attribs, attrib_count, values, a)

	int LibraryWGL::_ChoosePixelFormat(const OpenGLContextConfiguration* context_config, const OpenGLFramebufferConfiguration* fb_config)
	{
		OpenGLFramebufferConfiguration* usable_configs;
		const OpenGLFramebufferConfiguration* closest;
		int i, pixel_format, native_count, usable_count = 0, attrib_count = 0;
		int attribs[40];
		int values[sizeof(attribs) / sizeof(attribs[0])];

		platform::Window* window = ::GetMainWindow();

		native_count = ::DescribePixelFormat(window->context.dc, 1, sizeof(PIXELFORMATDESCRIPTOR), NULL);

		if (ARB_pixel_format)
		{
			ADD_ATTRIB(WGL_SUPPORT_OPENGL_ARB);
			ADD_ATTRIB(WGL_DRAW_TO_WINDOW_ARB);
			ADD_ATTRIB(WGL_PIXEL_TYPE_ARB);
			ADD_ATTRIB(WGL_ACCELERATION_ARB);
			ADD_ATTRIB(WGL_RED_BITS_ARB);
			ADD_ATTRIB(WGL_RED_SHIFT_ARB);
			ADD_ATTRIB(WGL_GREEN_BITS_ARB);
			ADD_ATTRIB(WGL_GREEN_SHIFT_ARB);
			ADD_ATTRIB(WGL_BLUE_BITS_ARB);
			ADD_ATTRIB(WGL_BLUE_SHIFT_ARB);
			ADD_ATTRIB(WGL_ALPHA_BITS_ARB);
			ADD_ATTRIB(WGL_ALPHA_SHIFT_ARB);
			ADD_ATTRIB(WGL_DEPTH_BITS_ARB);
			ADD_ATTRIB(WGL_STENCIL_BITS_ARB);
			ADD_ATTRIB(WGL_ACCUM_BITS_ARB);
			ADD_ATTRIB(WGL_ACCUM_RED_BITS_ARB);
			ADD_ATTRIB(WGL_ACCUM_GREEN_BITS_ARB);
			ADD_ATTRIB(WGL_ACCUM_BLUE_BITS_ARB);
			ADD_ATTRIB(WGL_ACCUM_ALPHA_BITS_ARB);
			ADD_ATTRIB(WGL_AUX_BUFFERS_ARB);
			ADD_ATTRIB(WGL_STEREO_ARB);
			ADD_ATTRIB(WGL_DOUBLE_BUFFER_ARB);

			if (ARB_multisample)
				ADD_ATTRIB(WGL_SAMPLES_ARB);

			if (ARB_framebuffer_sRGB || EXT_framebuffer_sRGB)
				ADD_ATTRIB(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);

			// NOTE: In a Parallels VM WGL_ARB_pixel_format returns fewer pixel formats than
			//       DescribePixelFormat, violating the guarantees of the extension spec
			// HACK: Iterate through the minimum of both counts

			const int attrib = WGL_NUMBER_PIXEL_FORMATS_ARB;
			int extension_count;

			if (!wglGetPixelFormatAttribivARB(window->context.dc, 1, 0, 1, &attrib, &extension_count))
			{
				Error("WGL: Failed to retrieve pixel format attribute");
				return 0;
			}

			if (native_count > extension_count) // take minimum
				native_count = extension_count;
		}

		usable_configs = new OpenGLFramebufferConfiguration[ native_count ];

		for (i = 0; i < native_count; ++i)
		{
			OpenGLFramebufferConfiguration* u = usable_configs + usable_count;
			pixel_format = i + 1;

			if (ARB_pixel_format)
			{
				// Get pixel format attributes through "modern" extension

				if (!wglGetPixelFormatAttribivARB(window->context.dc, pixel_format, 0, attrib_count, attribs, values))
				{
					Error("WGL: Failed to retrieve pixel format attributes");

					delete[] usable_configs;
					return 0;
				}

				if (!FIND_ATTRIB_VALUE(WGL_SUPPORT_OPENGL_ARB) ||
					!FIND_ATTRIB_VALUE(WGL_DRAW_TO_WINDOW_ARB))
				{
					continue;
				}

				if (FIND_ATTRIB_VALUE(WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB)
					continue;

				if (FIND_ATTRIB_VALUE(WGL_ACCELERATION_ARB) == WGL_NO_ACCELERATION_ARB)
					continue;

				const int doublebuffer = fb_config->doublebuffer ? 1 : 0;
				if (FIND_ATTRIB_VALUE(WGL_DOUBLE_BUFFER_ARB) != doublebuffer)
					continue;

				u->red_bits         = FIND_ATTRIB_VALUE(WGL_RED_BITS_ARB);
				u->green_bits       = FIND_ATTRIB_VALUE(WGL_GREEN_BITS_ARB);
				u->blue_bits        = FIND_ATTRIB_VALUE(WGL_BLUE_BITS_ARB);
				u->alpha_bits       = FIND_ATTRIB_VALUE(WGL_ALPHA_BITS_ARB);

				u->depth_bits       = FIND_ATTRIB_VALUE(WGL_DEPTH_BITS_ARB);
				u->stencil_bits     = FIND_ATTRIB_VALUE(WGL_STENCIL_BITS_ARB);

				u->accum_red_bits   = FIND_ATTRIB_VALUE(WGL_ACCUM_RED_BITS_ARB);
				u->accum_green_bits = FIND_ATTRIB_VALUE(WGL_ACCUM_GREEN_BITS_ARB);
				u->accum_blue_bits  = FIND_ATTRIB_VALUE(WGL_ACCUM_BLUE_BITS_ARB);
				u->accum_alpha_bits = FIND_ATTRIB_VALUE(WGL_ACCUM_ALPHA_BITS_ARB);

				u->aux_buffers      = FIND_ATTRIB_VALUE(WGL_AUX_BUFFERS_ARB);

				if (FIND_ATTRIB_VALUE(WGL_STEREO_ARB))
					u->stereo = true;

				if (ARB_multisample)
					u->samples = FIND_ATTRIB_VALUE(WGL_SAMPLES_ARB);

				if (ARB_framebuffer_sRGB ||
					EXT_framebuffer_sRGB)
				{
					if (FIND_ATTRIB_VALUE(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB))
						u->sRGB = true;
				}
			}
			else
			{
				// Get pixel format attributes through legacy PFDs
				PIXELFORMATDESCRIPTOR pfd;
				if (!::DescribePixelFormat(window->context.dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
				{
					Error("WGL: Failed to describe pixel format");

					delete[] usable_configs;
					return 0;
				}

				if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW) ||
					!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
				{
					continue;
				}

				if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) &&
					(pfd.dwFlags & PFD_GENERIC_FORMAT))
				{
					continue;
				}

				if (pfd.iPixelType != PFD_TYPE_RGBA)
					continue;

				if (!!(pfd.dwFlags & PFD_DOUBLEBUFFER) != fb_config->doublebuffer)
					continue;

				u->red_bits         = pfd.cRedBits;
				u->green_bits       = pfd.cGreenBits;
				u->blue_bits        = pfd.cBlueBits;
				u->alpha_bits       = pfd.cAlphaBits;

				u->depth_bits       = pfd.cDepthBits;
				u->stencil_bits     = pfd.cStencilBits;

				u->accum_red_bits   = pfd.cAccumRedBits;
				u->accum_green_bits = pfd.cAccumGreenBits;
				u->accum_blue_bits  = pfd.cAccumBlueBits;
				u->accum_alpha_bits = pfd.cAccumAlphaBits;

				u->aux_buffers      = pfd.cAuxBuffers;

				if (pfd.dwFlags & PFD_STEREO)
					u->stereo = true;
			}

			u->handle = pixel_format;
			++usable_count;
		}

		if (!usable_count)
		{
			Error("WGL: The driver does not appear to support OpenGL");

			delete[] usable_configs;
			return 0;
		}

		closest = ::ChooseFramebufferConfig(fb_config, usable_configs, usable_count);
		if (!closest)
		{
			Error("WGL: Failed to find a suitable pixel format");

			delete[] usable_configs;
			return 0;
		}

		pixel_format = closest->handle;
		delete[] usable_configs;

		return pixel_format;
	}

#undef ADD_ATTRIB
#undef FIND_ATTRIB_VALUE

} // namespace scythe