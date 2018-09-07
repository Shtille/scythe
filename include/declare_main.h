#ifndef __SCYTHE_DECLARE_MAIN_H__
#define __SCYTHE_DECLARE_MAIN_H__

#include "../src/common/platform.h"

#if defined(SCYTHE_TARGET_WINDOWS) || defined(SCYTHE_TARGET_MAC) || defined(SCYTHE_TARGET_LINUX)
# define SCYTHE_RENDERER_OPENGL // other isn't emplemented
#elif defined(SCYTHE_TARGET_IOS) || defined(SCYTHE_TARGET_ANDROID)
# define SCYTHE_RENDERER_OPENGLES
#endif

// Choose renderer
#if defined(SCYTHE_RENDERER_OPENGL)

# include "../src/application/opengl_application.h"

#elif defined(SCYTHE_RENDERER_OPENGLES)

# include "../src/application/opengles_application.h"

#elif defined(SCYTHE_RENDERER_DIRECTX)

# include "../src/application/directx_application.h"

#else

# pragma message("There is no renderer had been specified. Using OpenGL renderer as default.") 
# include "../src/application/opengl/opengl_application.h"

#endif

// Main function declaration macros
#if defined(SCYTHE_TARGET_WINDOWS)

# define DECLARE_MAIN(UserApplicationClass)				\
int WINAPI WinMain(HINSTANCE hInstance,					\
				   HINSTANCE hPrevInstance,				\
				   LPSTR lpCmdLine,						\
				   int nCmdShow)						\
{														\
UserApplicationClass *app = new UserApplicationClass();	\
int r = app->Run(0,0);									\
delete app;												\
return r;												\
}

#elif defined(SCYTHE_TARGET_LINUX) || defined(SCYTHE_TARGET_MAC)

# define DECLARE_MAIN(UserApplicationClass)				\
int main(int argc, const char ** argv)					\
{														\
UserApplicationClass *app = new UserApplicationClass();	\
int r = app->Run(argc, argv);							\
delete app;												\
return r;												\
}

#else
# error Main function hasn't been declared for this platform.
#endif

#endif