cmake_minimum_required(VERSION 3.20)

project(vulkan-stuff VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Vulkan
find_package(Vulkan REQUIRED)

add_executable(enumerate-gpus ${CMAKE_CURRENT_SOURCE_DIR}/enumerate_gpus.cpp)
target_link_libraries(enumerate-gpus PRIVATE Vulkan::Vulkan)

add_executable(gpu-memory-usage ${CMAKE_CURRENT_SOURCE_DIR}/gpu_memory_usage.cpp)
target_link_libraries(gpu-memory-usage PRIVATE Vulkan::Vulkan)

