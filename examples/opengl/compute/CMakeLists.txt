# CMakeLists file for compute example

project(compute VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
set(SRC_FILES
	drawer.cpp
	drawer.h
	main.cpp
	shaders.h
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
