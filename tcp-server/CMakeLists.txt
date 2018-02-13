file(GLOB SRCS "*.cc")
add_executable(TcpServer ${SRCS})
unset(SRCS)
