#ifndef __SCYTHE_OPENGL_INCLUDE_H__
#define __SCYTHE_OPENGL_INCLUDE_H__

#include "platform.h"

#ifndef SCYTHE_USE_OPENGL
# error "OpenGL should be enabled to use this header"
#endif

#ifdef SCYTHE_TARGET_WINDOWS
# include <glad/gl.h>
#elif defined(SCYTHE_TARGET_MAC)
# include <OpenGL/gl3.h>
# include <OpenGL/glext.h>
#else
# error "OpenGL includes haven't been defined"
#endif

#endif