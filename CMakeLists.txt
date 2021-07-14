cmake_minimum_required(VERSION 3.2.2)
project(the-crew)

set(CMAKE_MODULE_PATH ${the-crew_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${the-crew_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${the-crew_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${the-crew_SOURCE_DIR}/bin)

set(DEPS_BASE ${the-crew_SOURCE_DIR}/deps)

find_package(GMP)
find_package(Kissat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)
