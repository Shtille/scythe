# Makefile for scythe

# Platform-specific defines
STATIC_LIB_EXT = .a
AR = ar rcs
ifeq ($(OS),Windows_NT)
	#CCFLAGS += -D WIN32
	MAKE := mingw32-make.exe
	LDFLAGS = -s -shared
	CC = gcc
	CCP = g++
	PLATFORM_SUFFIX = mingw32
	SHARED_LIB_EXT = .so
else
	MAKE := make
	LDFLAGS = -shared -fPIC
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		#CCFLAGS += -D LINUX
		CC = gcc
		CCP = g++
		PLATFORM_SUFFIX = unix
		SHARED_LIB_EXT = .so
	endif
	ifeq ($(UNAME_S),Darwin)
		#CCFLAGS += -D OSX
		CC = clang
		CCP = clang++
		PLATFORM_SUFFIX = macosx
		SHARED_LIB_EXT = .dylib
		# OSX has its own CURL with command line tools
		CURL_LIB := curl
	endif
endif

# Exports
export STATIC_LIB_EXT
export SHARED_LIB_EXT
export CC
export CCP
export AR
export LDFLAGS

INSTALL_PATH = lib

# Main routine
SUBDIRS = thirdparty src

all: $(SUBDIRS)

create_dir:
	@test -d $(INSTALL_PATH) || mkdir $(INSTALL_PATH)

install: create_dir
	@find $(INSTALL_PATH) -name "*$(STATIC_LIB_EXT)" -type f -delete
	@find thirdparty -name "*$(STATIC_LIB_EXT)" -type f -exec mv {} $(INSTALL_PATH) \;

$(SUBDIRS):
	@echo Get down to $@
	@$(MAKE) -C $@

.PHONY: $(SUBDIRS)
