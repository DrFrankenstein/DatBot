cmake_minimum_required (VERSION 3.8)

project ("DatBot")

find_package(Boost 1.69.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory ("DatBot")
