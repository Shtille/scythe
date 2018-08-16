#ifndef __SCYTHE_OPENGL_INCLUDE_H__
#define __SCYTHE_OPENGL_INCLUDE_H__

#include "common/platform.h"

#ifdef SCYTHE_TARGET_WINDOWS
 #include "thirdparty/glew/include/GL/glew.h"
#elif defined(SCYTHE_TARGET_MAC)
 #include <OpenGL/gl3.h>
 #include <OpenGL/glext.h>
#else
 #error "OpenGL includes haven't been defined"
#endif

#endif