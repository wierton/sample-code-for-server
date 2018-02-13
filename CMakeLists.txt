cmake_minimum_required (VERSION 3.0)

project(SampleServer CXX)
set(CMAkE_CXX_STANDARD 14)

add_subdirectory(http-server)
add_subdirectory(http-static)
add_subdirectory(streambuf)
add_subdirectory(tcpstream)
add_subdirectory(tcp-server)
