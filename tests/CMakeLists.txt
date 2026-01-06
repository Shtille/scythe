project(test_scythe)

set(SRC_FILES
	main.cpp
)

set(LIBRARIES
	gtest::gtest
	scythe
)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
if (WIN32 AND SCYTHE_WINDOWS_NO_CONSOLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE OFF)
endif ()

add_test(${PROJECT_NAME} ${PROJECT_NAME})