cmake_minimum_required (VERSION 3.0)

project(SampleServer CXX)

macro(use_cxx14)
	if (CMAKE_VERSION VERSION_LESS "3.1")
		if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
		endif ()
	else ()
		set (CMAKE_CXX_STANDARD 14)
	endif ()
endmacro(use_cxx14)

use_cxx14()

add_subdirectory(http-server)
add_subdirectory(http-static)
add_subdirectory(streambuf)
add_subdirectory(tcpstream)
add_subdirectory(tcp-server)
