cmake_minimum_required(VERSION 3.5)
project( SkillIssue25-Raspi )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-W -Wall -Wextra -Werror)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package( OpenCV REQUIRED )
find_package( json-c REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${json-c_INCLUDE_DIRS} )

file(GLOB_RECURSE F_SOURCES "*.cpp")
file(GLOB_RECURSE F_HEADERS "*.hpp")

list(FILTER F_SOURCES EXCLUDE REGEX "/CMakeFiles/*")
list(FILTER F_HEADERS EXCLUDE REGEX "/CMakeFiles/*")

add_executable (SkillIssue25-Raspi ${F_HEADERS} ${F_SOURCES})

target_link_libraries( SkillIssue25-Raspi ${OpenCV_LIBS} )
target_link_libraries( SkillIssue25-Raspi ${json-c_LIBS} )


