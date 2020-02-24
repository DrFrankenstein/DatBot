cmake_minimum_required (VERSION 3.15)

project ("DatBot")

find_package(Boost 1.69.0 REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIR})

find_package(rxcpp REQUIRED)

find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIR})

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
	add_compile_definitions(_WIN32_WINNT=_WIN32_WINNT_WIN10)
	add_compile_definitions(_WINSOCK_DEPRECATED_NO_WARNINGS=1)
endif ()

# Workaround until rxcpp gets updated in vcpkg
# It uses std::unexpected which was removed in C++17
add_compile_definitions(_HAS_UNEXPECTED=1)

# Workaround until yaml-cpp becomes C++17 compliant
add_compile_definitions(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING=1)

if (MSVC)
	# level 4 warnings, no warnings in external headers
	# /Wall provides nothing useful over /W4, and warns on really insignificant things such as the existence of padding
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /experimental:external /external:anglebrackets /external:W0 ")
else ()
	# ALL THE WARNINGS
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic ")

	# Irc::Models::Message is designed to be built with partial initializers
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers ")
endif ()

add_subdirectory ("DatBot")

install(TARGETS "DatBot")

set(CPACK_PACKAGE_NAME "DatBot")
set(CPACK_PACKAGE_VENDOR "DrFrankenstein")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_DESCRIPTION "An IRC bot in development")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An IRC bot in development")
set(CPACK_PACKAGE_HOMEPAGE_URL "http://github.com/DrFrankenstein/DatBot/")

include(CPack)
