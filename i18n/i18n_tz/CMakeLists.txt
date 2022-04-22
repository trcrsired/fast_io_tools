cmake_minimum_required(VERSION 3.15)

project(generate_tz_file)

set(CMAKE_CXX_STANDARD 23)

add_executable(generate_tz_file parser.cc)
target_precompile_headers(generate_tz_file PUBLIC pch.hpp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
target_link_libraries(generate_tz_file ntdll)
endif()