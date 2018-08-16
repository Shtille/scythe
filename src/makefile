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

CFLAGS = -g -Wall -O3 -std=c++11
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

OBJECTS = $(SRC_FILES:.cpp=.o)

LIBRARIES =

all: $(SRC_FILES) $(TARGET)
	@echo $(TARGET_FILE) is done!

$(TARGET): clean $(TARGET_TYPE)

# TODO: Make clean platform dependent
clean:
	@find . -name "*.o" -type f -delete

static: $(OBJECTS)
	@echo making static library
	@$(AR) $(STATIC_LIB) $(OBJECTS)
	
dynamic: $(OBJECTS)
	@echo making shared library
	@$(CCP) $(LDFLAGS) -o $(SHARED_LIB) $^ $(LIBRARIES)

%.o : %.cpp
	@echo compiling file $<
	@$(CCP) $(CFLAGS) -c $< -o $@