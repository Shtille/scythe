# Source directories for scythe

SRC_DIRS = \
	src \
	src/application \
	src/common \
	src/containers \
	src/event \
	src/filesystem \
	src/graphics \
	src/graphics/opengl \
	src/model \
	src/image \
	src/input \
	src/math \
	src/math/geometry \
	src/memory \
	src/scene \
	src/stream \
	src/tasks \
	src/time \
	src/ui \
	src/planet \
	src/physics

# Includes
THIRDPARTY = ../thirdparty

INCLUDE = \
	-Isrc \
	-I$(THIRDPARTY)/libjpeg/include \
	-I$(THIRDPARTY)/libjpeg/src \
	-I$(THIRDPARTY)/libpng/include \
	-I$(THIRDPARTY)/libpng/src \
	-I$(THIRDPARTY)/freetype/include \
	-I$(THIRDPARTY)/bullet/src

# Platform specific

SRC_DIRS += src/platform
ifeq ($(OS),Windows_NT)
	# Windows
	SRC_DIRS += src/platform/windows
	INCLUDE += -I$(THIRDPARTY)/glew/include
	DEFINES += -DGLEW_STATIC
else
	# Posix based
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		# Linux
	endif
	ifeq ($(UNAME_S),Darwin)
		# Mac OS X
		SRC_DIRS += src/platform/macosx
		DEFINES += -DGL_SILENCE_DEPRECATION
	endif
endif
