cmake_minimum_required(VERSION 3.10)

project(FileDistributor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file (GLOB_RECURSE FileDist_SOURCES CONFIGURE_DEPENDS "lib/*.cpp")
#file (GLOB_RECURSE FileDist_HEADERS CONFIGURE_DEPENDS "lib/*.h")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


message(FILEDIST_HEADERS=${FileDist_HEADERS})
add_library(filedist STATIC ${FileDist_SOURCES})
target_include_directories(filedist PRIVATE lib/)


add_executable(testing "testing.cpp")
target_include_directories(testing PRIVATE lib/)
target_link_libraries(testing PUBLIC filedist uuid)


