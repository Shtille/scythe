# CMakeLists file for opengl examples directory

add_subdirectory(background)
if (SCYTHE_USE_MATH) # math is also required for these examples
	add_subdirectory(compute_filtering)
	add_subdirectory(compute_partition)
	add_subdirectory(polyline_performance)
endif (SCYTHE_USE_MATH)