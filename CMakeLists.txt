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

set(YAMLCPP_STATIC_LIBRARY TRUE)
find_package(YamlCpp)
if(YAMLCPP_FOUND)
    include_directories(${YAMLCPP_INCLUDE_DIR})
    target_link_libraries(crisppain ${YAMLCPP_LIBRARY})
endif()
find_package(Boost COMPONENTS system thread filesystem REQUIRED)
target_link_libraries(crisppain ${Boost_LIBRARIES})

find_package(OpenGL)
if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(crisppain ${OPENGL_LIBRARIES})
endif()

find_package(GLEW)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(crisppain ${GLEW_LIBRARIES})
endif()

find_package(ASSIMP)
if (ASSIMP_FOUND)
    include_directories(${ASSIMP_INCLUDE_DIRS})
    target_link_libraries(crisppain ${ASSIMP_LIBRARIES})
endif()

# Detect and add Qt
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
target_link_libraries(crisppain ${QT_LIBRARIES})

find_package(X11)
target_link_libraries(crisppain ${X11_LIBRARIES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
# Detect and add SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(crisppain ${SFML_LIBRARIES})
endif()

set(CRISPSYNTH_STATIC_LIBRARY TRUE)
find_package(CrispSynth)
if (CRISPSYNTH_FOUND)
    include_directories(${CRISPSYNTH_INCLUDE_DIR})
    target_link_libraries(crisppain ${CRISPSYNTH_LIBRARY})
endif()

