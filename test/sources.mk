
SRC_DIRS = \
	memory

SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Google Test files
SRC_FILES += $(ROOT_PATH)/thirdparty/googletest/src/gtest-all.cc
SRC_FILES += $(ROOT_PATH)/thirdparty/googletest/src/gtest_main.cc