cmake_minimum_required(VERSION 3.27)

project(zero)

set(CMAKE_C_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra)
target_include_directories(${PROJECT_NAME} PUBLIC include ${SDL2_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
target_precompile_headers(${PROJECT_NAME} PUBLIC include/${PROJECT_NAME}.h)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES})
