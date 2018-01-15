#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <map>
#include <string>
#include <regex>
#include <iostream>
#include <streambuf>

#include "tcpstream.h"


class TCPServer {
	int port;
	int servfd;

private:
	void init_servfd();

public:
	TCPServer(int port=80); // default to be port 80
	~TCPServer();

	void init_servfd(int port);
	TCPStream accept_client();
};

#endif
