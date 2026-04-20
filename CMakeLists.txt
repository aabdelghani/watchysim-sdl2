cmake_minimum_required(VERSION 3.16)
project(watchysim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

file(GLOB WATCHFACE_SRC CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/WatchySim/WatchFaces/*/*.cpp")

add_executable(watchysim
    WatchySim/main_qt.cpp
    WatchySim/Watchy.cpp
    WatchySim/arduino/String.cpp
    WatchySim/arduino/dtostrf.cpp
    ${WATCHFACE_SRC})

target_include_directories(watchysim PRIVATE
    ${CMAKE_SOURCE_DIR}/WatchySim
    ${CMAKE_SOURCE_DIR}/WatchySim/arduino)

target_link_libraries(watchysim PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core)

if(NOT MSVC)
    target_compile_options(watchysim PRIVATE -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-narrowing)
endif()
