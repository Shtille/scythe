# CMakeLists file for scythe
# requires SCYTHE_THIRDPARTY_DIR to be set

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(scythe)

set(CMAKE_CXX_STANDARD 11)
set(SRC_DIRS
  src
  src/application
  src/common
  src/containers
  src/event
  src/filesystem
  src/graphics
  src/graphics/opengl
  src/model
  src/image
  src/input
  src/math
  src/math/geometry
  src/memory
  src/scene
  src/stream
  src/tasks
  src/time
  src/ui
  src/planet
  src/platform
  src/physics
)
if ("${SCYTHE_THIRDPARTY_DIR}" STREQUAL "")
  message(ERROR "SCYTHE_THIRDPARTY_DIR should be set")
endif()
set(include_directories 
  src
  ${SCYTHE_THIRDPARTY_DIR}/libjpeg/include
  ${SCYTHE_THIRDPARTY_DIR}/libjpeg/src
  ${SCYTHE_THIRDPARTY_DIR}/libpng/include
  ${SCYTHE_THIRDPARTY_DIR}/libpng/src
  ${SCYTHE_THIRDPARTY_DIR}/freetype/include
  ${SCYTHE_THIRDPARTY_DIR}/bullet/src
)
#set(defines )
set(libraries
  freetype
  jpeg
  png
  z
)

# Platform specific
if (WIN32)
  # Windows
  set(SRC_DIRS ${SRC_DIRS} src/platform/windows)
  set(include_directories ${include_directories} ${SCYTHE_THIRDPARTY_DIR}/glew/include)
  set(defines ${defines} GLEW_STATIC)
  set(libraries ${libraries} glew gdi32 opengl32)
else()
  # Posix based
  if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    # Mac OS X
    set(DARWIN_SRC_DIRS src/platform/macosx)
    set(defines ${defines} GL_SILENCE_DEPRECATION)
    set(libraries ${libraries} "-framework Cocoa -framework OpenGL -framework Foundation")
  else()
    # Linux
  endif()
endif()

foreach(DIR ${SRC_DIRS})
  file(GLOB DIR_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.cpp)
  set(SRC_FILES ${SRC_FILES} ${DIR_SOURCE})
endforeach(DIR)
foreach(DIR ${DARWIN_SRC_DIRS})
  file(GLOB DIR_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.m ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.mm)
  set(SRC_FILES ${SRC_FILES} ${DIR_SOURCE})
endforeach(DIR)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${include_directories})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${defines})
target_link_libraries(${PROJECT_NAME} PUBLIC ${libraries})
