# Source directories for scythe

SRC_DIRS = \
	. \
	application \
	common \
	containers \
	event \
	filesystem \
	graphics \
	graphics/opengl \
	image \
	input \
	math \
	math/geometry \
	scene \
	stream \
	tasks \
	time \
	ui

# Includes
THIRDPARTY = ../thirdparty

INCLUDE = \
	-I$(THIRDPARTY)/libjpeg/include \
	-I$(THIRDPARTY)/libjpeg/src \
	-I$(THIRDPARTY)/libpng/include \
	-I$(THIRDPARTY)/libpng/src \
	-I$(THIRDPARTY)/freetype/include

# Platform specific

SRC_DIRS += platform
ifeq ($(OS),Windows_NT)
	# Windows
	SRC_DIRS += platform/windows
	DEFINES += -DGLEW_STATIC
else
	# Posix based
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		# Linux
	endif
	ifeq ($(UNAME_S),Darwin)
		# Mac OS X
		SRC_DIRS += platform/macosx
	endif
endif