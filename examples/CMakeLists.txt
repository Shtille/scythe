# CMakeLists file for examples directory

add_subdirectory(common)
if (SCYTHE_USE_OPENGL)
	add_subdirectory(opengl)
endif (SCYTHE_USE_OPENGL)