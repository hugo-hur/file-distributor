cmake_minimum_required(VERSION 3.10)

project(FileDistributor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file (GLOB_RECURSE FileDist_SOURCES CONFIGURE_DEPENDS "lib/*.cpp")
file (GLOB_RECURSE FileDist_HEADERS CONFIGURE_DEPENDS "lib/*.h")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(filedist STATIC ${FileDist_SOURCES})
target_include_directories(FileDist PRIVATE ${FileDist_INCLUDE_DIRS})


add_executable(testing "testing.cpp")
target_link_libraries(testing PUBLIC filedist)
target_include_directories(testing PRIVATE ${FileDist_INCLUDE_DIRS})

