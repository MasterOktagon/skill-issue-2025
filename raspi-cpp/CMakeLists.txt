cmake_minimum_required(VERSION 3.5)
project( SkillIssue25-Raspi )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-W -Wall -Wextra -Werror)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE F_SOURCES "*.cpp")
#file(GLOB_RECURSE F_HEADERS "*.hpp")
#message(${F_SOURCES})
list(REMOVE_ITEM F_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM F_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles/3.22.1/CompilerIdCXX/CMakeCXXCompilerId.cpp/main.cpp")
message(${F_SOURCES})

add_executable (SkillIssue25-Raspi main.cpp kmeans.hpp kmeans.cpp)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( SkillIssue25-Raspi ${OpenCV_LIBS} )
#find_package( i2c REQUIRED )
#include_directories( ${i2c_INCLUDE_DIRS} )
target_link_libraries( SkillIssue25-Raspi i2c )

