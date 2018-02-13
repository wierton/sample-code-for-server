file(GLOB SRCS "*.cc")
add_executable(TcpStream ${SRCS})
unset(SRCS)
