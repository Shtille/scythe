# CMakeLists file for compute partition example

project(compute_partition VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
set(SRC_FILES
	drawer.cpp
	drawer.h
	partitioner.cpp
	partitioner.h
	main.cpp
	shaders.h
	partition_shaders.h
)

# Libraries
set(LIBRARIES
	scythe
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES 
    DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
