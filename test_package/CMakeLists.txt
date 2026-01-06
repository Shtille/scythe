cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

find_package(scythe CONFIG REQUIRED)

add_executable(example src/main.cpp)
target_link_libraries(example scythe::scythe)
