# CMake file scythe

project(scythe VERSION 2.0.0 LANGUAGES CXX)

# Public headers
set(PUBLIC_HEADERS
	./include/scythe/application.h
	./include/scythe/clock.h
	./include/scythe/defines.h
	./include/scythe/desktop_application.h
	./include/scythe/endianness.h
	./include/scythe/expandable.h
	./include/scythe/flags.h
	./include/scythe/graphics_controller.h
	./include/scythe/graphics_provider.h
	./include/scythe/keyboard.h
	./include/scythe/keyboard_controller.h
	./include/scythe/log.h
	./include/scythe/logics_controller.h
	./include/scythe/main.h
	./include/scythe/mouse.h
	./include/scythe/mouse_controller.h
	./include/scythe/non_copyable.h
	./include/scythe/physics_controller.h
	./include/scythe/platform.h
	./include/scythe/platform_includes.h
	./include/scythe/resource.h
	./include/scythe/resource_manager.h
	./include/scythe/singleton.h
	./include/scythe/string_format.h
	./include/scythe/string_id.h
	./include/scythe/time_manager.h
	./include/scythe/timer.h
	./include/scythe/types.h
	./include/scythe/window.h
)

# Source files
set(SRC_FILES
	./src/application/application.cpp
	./src/application/desktop_application.cpp
	./src/graphics/graphics_provider.cpp
	./src/input/keyboard.cpp
	./src/input/mouse.cpp
	./src/platform/platform_inner.h
	./src/time/clock.cpp
	./src/time/time_manager.cpp
	./src/time/timer.cpp
	./src/endianness.cpp
	./src/log.cpp
	./src/resource_manager.cpp
	./src/string_id.cpp
)
# Windows specific sources
if (WIN32)
	list(APPEND SRC_FILES
		./src/platform/windows/platform_inner.cpp
		./src/platform/windows/platform_window.h
	)
endif (WIN32)

# Libraries
set(LIBRARIES
	# TODO
)

# Compile definitions
set(PRIVATE_DEFINES
	_CRT_SECURE_NO_WARNINGS
)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${PUBLIC_HEADERS})
target_include_directories(${PROJECT_NAME} 
						   PUBLIC 
						   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include"
						   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PRIVATE_DEFINES})
#set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${SCYTHE_WORKING_DIRECTORY})

# Pass options as defines
if (SCYTHE_USE_PHYSICS)
	target_compile_definitions(${PROJECT_NAME} PUBLIC SCYTHE_USE_PHYSICS)
endif (SCYTHE_USE_PHYSICS)
if (SCYTHE_WINDOWS_NO_CONSOLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC SCYTHE_WINDOWS_NO_CONSOLE)
endif (SCYTHE_WINDOWS_NO_CONSOLE)

# Installation properties
install(TARGETS ${PROJECT_NAME})
install(DIRECTORY include/scythe DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})