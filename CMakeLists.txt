cmake_minimum_required(VERSION 3.6)
project(crisppain)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_compile_options(-Wall)

file(GLOB_RECURSE SOURCE_FILES crisppain_src
        "src/*.h"
        "src/*.cpp"
)

add_executable(crisppain ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Detect and add Qt
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

target_link_libraries(crisppain ${QT_LIBRARIES})
