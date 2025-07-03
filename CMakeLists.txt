cmake_minimum_required(VERSION 3.10)
project(noisenow C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED true)

find_package(OpenAL REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/**.c")

add_executable(noisenow)
target_sources(noisenow PRIVATE ${PROJECT_SOURCES})
target_link_libraries(noisenow PRIVATE OpenAL::OpenAL)
