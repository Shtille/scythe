
SRC_DIRS = \
	containers \
	memory

SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Holds run all tests routine
SRC_FILES += main.cpp