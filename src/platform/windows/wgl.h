#ifndef __SCYTHE_WGL_H__
#define __SCYTHE_WGL_H__

#include <scythe/singleton.h>
#include <scythe/platform_includes.h>

#include "../../graphics/opengl/opengl_context_config.h"
#include "../../graphics/opengl/opengl_framebuffer_config.h"

// WGL specific defines
#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_TYPE_RGBA_ARB 0x202b
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_NO_ACCELERATION_ARB 0x2025
#define WGL_RED_BITS_ARB 0x2015
#define WGL_RED_SHIFT_ARB 0x2016
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_GREEN_SHIFT_ARB 0x2018
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_BLUE_SHIFT_ARB 0x201a
#define WGL_ALPHA_BITS_ARB 0x201b
#define WGL_ALPHA_SHIFT_ARB 0x201c
#define WGL_ACCUM_BITS_ARB 0x201d
#define WGL_ACCUM_RED_BITS_ARB 0x201e
#define WGL_ACCUM_GREEN_BITS_ARB 0x201f
#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_AUX_BUFFERS_ARB 0x2024
#define WGL_STEREO_ARB 0x2012
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_SAMPLES_ARB 0x2042
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20a9
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB 0x31b3
#define WGL_COLORSPACE_EXT 0x309d
#define WGL_COLORSPACE_SRGB_EXT 0x3089

// WGL extension pointer typedefs
typedef BOOL (WINAPI * PFN_wglSwapIntervalEXT)(int);
typedef BOOL (WINAPI * PFN_wglGetPixelFormatAttribivARB)(HDC,int,int,UINT,const int*,int*);
typedef const char* (WINAPI * PFN_wglGetExtensionsStringEXT)(void);
typedef const char* (WINAPI * PFN_wglGetExtensionsStringARB)(HDC);
typedef HGLRC (WINAPI * PFN_wglCreateContextAttribsARB)(HDC,HGLRC,const int*);

// opengl32.dll function pointer typedefs
typedef HGLRC (WINAPI * PFN_wglCreateContext)(HDC);
typedef BOOL (WINAPI * PFN_wglDeleteContext)(HGLRC);
typedef PROC (WINAPI * PFN_wglGetProcAddress)(LPCSTR);
typedef HDC (WINAPI * PFN_wglGetCurrentDC)(void);
typedef HGLRC (WINAPI * PFN_wglGetCurrentContext)(void);
typedef BOOL (WINAPI * PFN_wglMakeCurrent)(HDC,HGLRC);
typedef BOOL (WINAPI * PFN_wglShareLists)(HGLRC,HGLRC);

namespace scythe {

	/**
	 * @brief      This class describes a library WGL.
	 */
	class LibraryWGL final
	: public Singleton<LibraryWGL>
	{
	public:
		bool Initialize();
		void Deinitialize();

		bool CreateContext(const OpenGLContextConfiguration* context_config, const OpenGLFramebufferConfiguration* fb_config);
		void DestroyContext();

		bool IsExtensionSupported(const char* extension);

	private:
		int _ChoosePixelFormat(const OpenGLContextConfiguration* context_config, const OpenGLFramebufferConfiguration* fb_config);

	public:
		PFN_wglCreateContext                wglCreateContext;
		PFN_wglDeleteContext                wglDeleteContext;
		PFN_wglGetProcAddress               wglGetProcAddress;
		PFN_wglGetCurrentDC                 wglGetCurrentDC;
		PFN_wglGetCurrentContext            wglGetCurrentContext;
		PFN_wglMakeCurrent                  wglMakeCurrent;
		PFN_wglShareLists                   wglShareLists;

		PFN_wglSwapIntervalEXT              wglSwapIntervalEXT;
		PFN_wglGetPixelFormatAttribivARB    wglGetPixelFormatAttribivARB;
		PFN_wglGetExtensionsStringEXT       wglGetExtensionsStringEXT;
		PFN_wglGetExtensionsStringARB       wglGetExtensionsStringARB;
		PFN_wglCreateContextAttribsARB      wglCreateContextAttribsARB;

		bool                                EXT_swap_control;
		bool                                EXT_colorspace;
		bool                                ARB_multisample;
		bool                                ARB_framebuffer_sRGB;
		bool                                EXT_framebuffer_sRGB;
		bool                                ARB_pixel_format;
		bool                                ARB_create_context;
		bool                                ARB_create_context_profile;
		bool                                EXT_create_context_es2_profile;
		bool                                ARB_create_context_robustness;
		bool                                ARB_create_context_no_error;
		bool                                ARB_context_flush_control;

	private:
		HINSTANCE                           library_instance_;
	};

} // namespace scythe

#endif