file(GLOB SRCS "*.cc")
add_executable(HttpServer ${SRCS})
unset(SRCS)

target_link_libraries(HttpServer pthread)
