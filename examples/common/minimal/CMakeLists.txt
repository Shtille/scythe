# CMakeLists file for minimal example

project(minimal VERSION 0.1.0 LANGUAGES CXX)

# Sources
set(SRC_FILES
	main.cpp
)

# Libraries
set(LIBRARIES
	scythe
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})