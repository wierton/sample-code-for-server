#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <map>
#include <cctype>
#include <algorithm>
#include <memory>

#include "debug.h"
#include "tcpserver.h"



TCPServer::TCPServer(int port) :
	port(port),
	servfd(-1)
{
	init_servfd();
}


TCPServer::~TCPServer() {
	if(servfd > 0) {
		wlog("close server fd %\n", servfd);
		close(servfd);
	}
}

void TCPServer::init_servfd() {
	servfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servfd < 0) {
		wloge("Create Server Socket Failed!\n");
	}

	// create sockaddr
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		wloge("Can not bind to port %!\n", port);
	}

	// FIXME:
	if(listen(servfd, 50) == -1) {
		wloge("fail to listen on socket.\n");
	}
}


void TCPServer::init_servfd(int port) {
	// re-init
	this->~TCPServer();
	this->port = port;
	init_servfd();
}


TCPStream TCPServer::accept_client() {
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	int conn = accept(servfd, (struct sockaddr*)&client_addr, &length);
	wlog("connected by %:%, conn:%\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, conn);
	if(conn < 0) {
		wloge("fail to accept client.\n");
	}

	return TCPStream(conn);
}

