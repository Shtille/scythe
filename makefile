# Makefile for scythe
ifeq ($(LIBRARY_PATH),)
	$(warning LIBRARY_PATH should be set by parent makefile with absolute path)
endif

TARGET = scythe
LIBRARY_PATH := ../lib
STATIC_LIB = $(LIBRARY_PATH)/lib$(TARGET)$(STATIC_LIB_EXT)
SHARED_LIB = $(LIBRARY_PATH)/lib$(TARGET)$(SHARED_LIB_EXT)

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

include sources.mk

SRC_FILES_M = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.m))
SRC_FILES_MM = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.mm))
SRC_FILES_CPP = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
SRC_FILES = $(SRC_FILES_M) $(SRC_FILES_MM) $(SRC_FILES_CPP)

# intermediate directory for generated object files
OBJDIR := .o
# intermediate directory for generated dependency files
DEPDIR := .d

# object files, auto generated from source files
OBJECTS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRC_FILES)))
# dependency files, auto generated from source files
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRC_FILES)))

# compilers (at least gcc and clang) don't create the subdirectories automatically
$(shell mkdir -p $(dir $(OBJECTS)) >/dev/null)
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)

# C flags
CFLAGS := -std=c11
# C++ flags
CXXFLAGS := -std=c++11
# C/C++ flags
CPPFLAGS := -g -Wall -O3
#CPPFLAGS += -Wextra -pedantic
CPPFLAGS += $(INCLUDE)
CPPFLAGS += $(DEFINES)
# linker flags
LDFLAGS += -L$(LIBRARY_PATH)
LDLIBS = -lstdc++ -lfreetype -ljpeg -lpng -lz
ifeq ($(OS),Windows_NT)
	LDLIBS += -lgdi32 -lglew -lopengl32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		# TODO: Linux-specific libraries
	endif
	ifeq ($(UNAME_S),Darwin)
		LDLIBS += -framework Cocoa -framework OpenGL -framework Foundation
	endif
endif
# flags required for dependency generation; passed to compilers
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td

# compile C source files
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# compile C++ source files
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
# link object files to static/shared library
ifeq ($(IS_STATIC),NO)
	LINK.o = $(CXX) $(LDFLAGS) $(LDLIBS) -o $@
else
	LINK.o = $(AR) $@
endif
# precompile step
PRECOMPILE =
# postcompile step
ifeq ($(OS),Windows_NT)
	POSTCOMPILE = MOVE /Y $(DEPDIR)\$*.Td $(DEPDIR)\$*.d
else
	POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d
endif

ifeq ($(OS),Windows_NT)
	CLEAN = rmdir /Q /S $(OBJDIR) && rmdir /Q /S $(DEPDIR)
else
	CLEAN = rm -r $(OBJDIR) $(DEPDIR)
endif

all: $(TARGET)

.PHONY: clean
clean:
	@$(CLEAN)

.PHONY: install
install:
	@echo no install tasks configured

.PHONY: uninstall
uninstall:
	@echo no uninstall tasks configured

.PHONY: help
help:
	@echo available targets: all clean

$(TARGET): $(TARGET_FILE)

$(TARGET_FILE): $(OBJECTS)
	@echo linking $@
	@$(LINK.o) $(OBJECTS)

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	@$(PRECOMPILE)
	@echo compiling $<
	@$(COMPILE.c) $<
	@$(POSTCOMPILE)

$(OBJDIR)/%.o: %.m
$(OBJDIR)/%.o: %.m $(DEPDIR)/%.d
	@$(PRECOMPILE)
	@echo compiling $<
	@$(COMPILE.c) $<
	@$(POSTCOMPILE)

$(OBJDIR)/%.o: %.mm
$(OBJDIR)/%.o: %.mm $(DEPDIR)/%.d
	@$(PRECOMPILE)
	@echo compiling $<
	@$(COMPILE.cc) $<
	@$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	@$(PRECOMPILE)
	@echo compiling $<
	@$(COMPILE.cc) $<
	@$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cc
$(OBJDIR)/%.o: %.cc $(DEPDIR)/%.d
	@$(PRECOMPILE)
	@echo compiling $<
	@$(COMPILE.cc) $<
	@$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cxx
$(OBJDIR)/%.o: %.cxx $(DEPDIR)/%.d
	@$(PRECOMPILE)
	@echo compiling $<
	@$(COMPILE.cc) $<
	@$(POSTCOMPILE)

.PRECIOUS = $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(DEPS)