# CMake file to setup Conan

execute_process(
	COMMAND conan install ${CMAKE_SOURCE_DIR} --build=missing -s compiler.cppstd=17
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
	message(FATAL_ERROR "Conan install failed")
endif()