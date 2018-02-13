file(GLOB SRCS "*.cc")
add_executable(HttpStatic ${SRCS})
unset(SRCS)
