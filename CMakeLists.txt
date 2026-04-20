cmake_minimum_required(VERSION 3.16)
project(watchysim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)

file(GLOB WATCHFACE_SRC CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/WatchySim/WatchFaces/*/*.cpp")

add_executable(watchysim
    WatchySim/main_sdl.cpp
    WatchySim/Watchy.cpp
    WatchySim/arduino/String.cpp
    WatchySim/arduino/dtostrf.cpp
    ${WATCHFACE_SRC})

target_include_directories(watchysim PRIVATE
    ${CMAKE_SOURCE_DIR}/WatchySim
    ${CMAKE_SOURCE_DIR}/WatchySim/arduino
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(watchysim PRIVATE ${SDL2_LIBRARIES})

if(NOT MSVC)
    target_compile_options(watchysim PRIVATE -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-narrowing)
endif()
