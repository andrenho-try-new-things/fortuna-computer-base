cmake_minimum_required(VERSION 3.28)

project(fiob-emulator CXX)

find_package(SDL3 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fiob-emulator main.cc)

target_link_libraries(fiob-emulator PRIVATE SDL3::SDL3)

target_include_directories(fiob-emulator PRIVATE ${SDL3_INCLUDE_DIRS})