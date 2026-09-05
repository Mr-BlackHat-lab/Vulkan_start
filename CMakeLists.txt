cmake_minimum_required(VERSION 4.3)
project(try)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(try)

target_sources(try PRIVATE main.cpp)

target_link_libraries(try PRIVATE Vulkan::Vulkan)