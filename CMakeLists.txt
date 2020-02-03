cmake_minimum_required (VERSION 3.8)

project ("DatBot")

find_package(Boost 1.69.0 REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIR})

find_package(rxcpp REQUIRED)
find_path(rxcpp_INCLUDE_DIR NAMES rx.hpp)
message("rxcpp include dir:")
message("${rxcpp_INCLUDE_DIR}")
include_directories(${rxcpp_INCLUDE_DIR}/..)

find_package(yaml-cpp REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(_WIN32_WINNT=_WIN32_WINNT_WIN10)
add_compile_definitions(_WINSOCK_DEPRECATED_NO_WARNINGS=1)

# Workaround until rxcpp gets updated in vcpkg
# It uses std::unexpected which was removed in C++17
add_compile_definitions(_HAS_UNEXPECTED=1)

# Workaround until yaml-cpp becomes C++17 compliant
add_compile_definitions(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING=1)

if (MSVC)
	# yaml-cpp headers generate a lot of those warnings, and they don't seem to matter at this point
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275 ")
endif ()

add_subdirectory ("DatBot")
