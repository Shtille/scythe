# Makefile for scythe

TARGET = scythe
STATIC_LIB = lib$(TARGET)$(STATIC_LIB_EXT)
SHARED_LIB = lib$(TARGET)$(SHARED_LIB_EXT)

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

include sources.mk

CFLAGS = -g -Wall -O3
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)
CFLAGS_CPP = $(CFLAGS) -std=c++11

SRC_FILES_M = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.m))
SRC_FILES_MM = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.mm))
SRC_FILES_CPP = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
SRC_FILES = $(SRC_FILES_M) $(SRC_FILES_MM) $(SRC_FILES_CPP)

OBJECTS_M = $(SRC_FILES_M:.m=.o)
OBJECTS_MM = $(SRC_FILES_MM:.mm=.o)
OBJECTS_CPP = $(SRC_FILES_CPP:.cpp=.o)
OBJECTS = $(OBJECTS_M) $(OBJECTS_MM) $(OBJECTS_CPP)

LIBRARIES =

all: $(SRC_FILES) $(TARGET)
	@echo $(TARGET_FILE) is done!

$(TARGET): clean $(TARGET_TYPE)

# TODO: Make clean platform dependent
clean:
	@find src -name "*.o" -type f -delete

static: $(OBJECTS)
	@echo making static library
	@$(AR) $(STATIC_LIB) $(OBJECTS)
	
dynamic: $(OBJECTS)
	@echo making shared library
	@$(CCP) $(LDFLAGS) -o $(SHARED_LIB) $^ $(LIBRARIES)

%.o : %.m
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@

%.o : %.mm
	@echo compiling file $<
	@$(CCP) $(CFLAGS_CPP) -c $< -o $@

%.o : %.cpp
	@echo compiling file $<
	@$(CCP) $(CFLAGS_CPP) -c $< -o $@
