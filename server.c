#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {

	int enable = 1;
	int server_fd;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	struct sockaddr_in caddr;
	socklen_t caddrlen = sizeof(caddr);
	char buff[BUFFER_SIZE];
	char resp[] = "HTTP/1.0 200 OK \r\n"
		"Server: server-c\r\n"
		"Content-type: text/html\r\n\r\n"
		"<html>Basic</html>\r\n";
		


	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	printf("socket creation success\n");

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)) < 0) {
		perror("Set sock failed\n");
		exit(EXIT_FAILURE);
	}
	
	printf("sock opts set\n");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	
	if (bind(server_fd, (struct sockaddr *) &addr, addrlen) < 0) {
		perror("Bind failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Bind success\n");

	if(listen(server_fd, SOMAXCONN) < 0) {
		perror("Listen failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Listening\n");
	
	while (true) {
		int newsock_fd = accept(server_fd, (struct sockaddr *) &addr, (socklen_t *)&addrlen); 		
		if (newsock_fd < 0) {
			perror("Accept failed\n");
			continue;
		}

		printf("Connection success\n");

		int sockn = getsockname(newsock_fd, (struct sockaddr*) &caddr, (socklen_t *)&caddrlen);
		if (sockn < 0) {
			perror("Get client sock failed\n");
		}

		int valread = read(newsock_fd, buff, BUFFER_SIZE);
		if (valread < 0) {
			perror("Read failed\n");
			continue;
		}

		char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
		sscanf(buff, "%s %s %s", method, uri, version);
		printf("[%s:%u] %s %s %s\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port),
				method, uri, version);

		int valwrite = write(newsock_fd, resp, strlen(resp));
		if (valwrite < 0) {
			perror("Write failed\n");
			continue;
		}

		close(newsock_fd);
		
	}

	return 0;
}
