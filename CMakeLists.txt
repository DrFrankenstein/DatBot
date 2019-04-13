cmake_minimum_required (VERSION 3.8)

project ("DatBot")

find_package(Boost 1.69.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_compile_definitions(_WIN32_WINNT=_WIN32_WINNT_WIN10)

add_subdirectory ("DatBot")
