#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

#include <iostream>

int servfd = -1;

void sigint_handler(int signum) {
	printf("keyboard interrupt detected, close server\n");
	close(servfd);
	exit(0);
}

int main() {
	servfd = socket(AF_INET, SOCK_STREAM, 0);

	signal(SIGINT, sigint_handler);

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8080);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(servfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	listen(servfd, 50);
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	int conn = -1;
	while((conn = accept(servfd, (struct sockaddr*)&client_addr, &length)) != -1) {
		const char response[] = "Hello World\n";
		[[maybe_unused]]
		int len = write(conn, response, sizeof(response));
		close(conn);
	}

	return 0;
}
