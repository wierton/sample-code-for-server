#include "tcpstream.h"
#include "tcpserver.h"


int main() {
	TCPServer server(8080);
	while(auto client = server.accept_client()) {
		client << "HTTP/1.1 200 OK\n"
			"Content-Length: 11\n"
			"\n"
			"Hello World\n";
	}
	return 0;
}
