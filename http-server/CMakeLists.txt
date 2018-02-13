file(GLOB SRCS "*.cc")
add_executable(HttpServer ${SRCS})
unset(SRCS)
