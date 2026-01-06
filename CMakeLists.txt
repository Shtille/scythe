# Root CMakeLists file for scythe
cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

# C++17 standard requirement
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan configuration (in development)
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_SOURCE_DIR}/conan_setup.cmake" CACHE STRING "Conan provider module")

# Options
option(SCYTHE_BUILD_EXAMPLES "Build examples" ON)
option(SCYTHE_USE_PHYSICS "Use physics" ON)
if (WIN32)
	option(SCYTHE_WINDOWS_NO_CONSOLE "Native Windows GUI application" ON)
endif (WIN32)

# Configure build
if (WIN32)
	if (SCYTHE_WINDOWS_NO_CONSOLE)
		set(CMAKE_WIN32_EXECUTABLE ON)
	else (SCYTHE_WINDOWS_NO_CONSOLE)
		set(CMAKE_WIN32_EXECUTABLE OFF)
	endif (SCYTHE_WINDOWS_NO_CONSOLE)
endif (WIN32)

# Build dependencies

# Build scythe
include(scythe.cmake)

# Build unit tests
if (NOT BUILD_TESTING STREQUAL OFF)
	enable_testing()
	add_subdirectory(tests)
endif()

# Build examples
if (SCYTHE_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()