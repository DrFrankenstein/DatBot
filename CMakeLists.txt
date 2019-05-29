cmake_minimum_required (VERSION 3.8)

project ("DatBot")

find_package(Boost 1.69.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(_WIN32_WINNT=_WIN32_WINNT_WIN10)

# Workaround until rxcpp gets updated in vcpkg
# It uses std::unexpected which was removed in C++17
add_compile_definitions(_HAS_UNEXPECTED=1)

add_subdirectory ("DatBot")
