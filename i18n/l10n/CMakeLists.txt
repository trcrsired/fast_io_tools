cmake_minimum_required(VERSION 3.15)

project(generate_locale_file)

set(CMAKE_CXX_STANDARD 23)

add_executable(parse_locale_data parse_locale_data.cc)
target_precompile_headers(parse_locale_data PUBLIC pch.hpp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
target_link_libraries(parse_locale_data ntdll)
endif()