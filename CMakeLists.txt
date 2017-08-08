cmake_minimum_required(VERSION 3.0.0)
project(nhtml C)

add_subdirectory(src)
add_executable(nhtmlc ${SOURCE})
target_include_directories(nhtmlc PUBLIC inc)

if (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    message("debug")
    target_compile_definitions(nhtmlc PUBLIC DEBUG)
endif (CMAKE_BUILD_TYPE MATCHES "DEBUG") 

target_link_libraries(nhtmlc)
