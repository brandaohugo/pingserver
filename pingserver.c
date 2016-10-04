#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define TRUE 1
#define PORT_NUMBER 1234
#define BUFFER_SIZE 1500
#define TIMEOUT_SECS 0
#define TIMEOUT_USECS 100000

void check_arguments(int argc){
	if (argc != 1) {
		fprintf(stderr,"Usage: pingserver");
		exit(1);
	}
}

int create_socket(){
	int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(fd < 0) {
		fprintf(stderr, "Error while creating the socket: %s\n", strerror(errno));
		exit(1);
	}
	return fd;
}

void bind_socket(int fd) {
	int err;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUMBER);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	err = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	if (err < 0) {
		fprintf(stderr, "Error while assigning address to the socket: %s\n", strerror(errno));
		exit(1);
	}
}

void send_response(int fd, char *buff, struct sockaddr_in dest){
	int err;
	char msg[BUFFER_SIZE];

	err = sendto(fd, buff, BUFFER_SIZE, 0, (struct sockaddr*) &dest, sizeof(struct sockaddr_in));
	if ( err < 0) {
		fprintf(stderr, "Error while sending response message: %s\n" , strerror(errno));		
	} else {
		sprintf(msg, "Sent %d bytes to host %s port %d: %s", err, inet_ntoa(dest.sin_addr), ntohs(dest.sin_port), buff);
		fprintf(stdout, "%s\n", msg);
	}
}

void listen_port(int fd) {

	int err,nb;
	//TODO: check buffer and message sizes
	//TODO: clean the buffer
	char buff[BUFFER_SIZE];
	char msg[BUFFER_SIZE];
	socklen_t flen;
	struct sockaddr_in from;
	fd_set read_set;
	struct timeval timeout;
	
	FD_ZERO(&read_set);
	FD_SET(fd, &read_set);
	timeout.tv_sec = TIMEOUT_SECS;
	timeout.tv_usec = TIMEOUT_USECS;

	nb = select(fd+1, &read_set, NULL, NULL, &timeout);
	if (nb < 0) {
		fprintf(stderr, "Error while setting timeout: %s\n", strerror(errno));		
	}
	if (nb == 0) {
		//fprintf(stderr, "Timeout\n");			
	}
	if (FD_ISSET(fd, &read_set)){
		flen = sizeof(struct sockaddr_in);
		err = recvfrom( fd, buff, BUFFER_SIZE, 0, (struct sockaddr *) &from, &flen);
		
		if (err < 0)	 {
			fprintf(stderr, "Error while receiving message: %s\n", strerror(errno));		
		} else {
			sprintf(msg, "Received %d bytes from host %s port %d: %s", err, inet_ntoa(from.sin_addr), ntohs(from.sin_port), buff);
			fprintf(stdout, "%s\n", msg);
			send_response(fd, (char *) &buff, from);
		}	
	}	
}

int main(int argc, char** argv) {
	
	int socket_fd,err;	
		
	check_arguments(argc);
	
	socket_fd = create_socket();

	bind_socket(socket_fd);

	while(TRUE){
		
		listen_port(socket_fd);

	}

	err = close(socket_fd);
	if( err < 0){
		fprintf(stderr, "Error while closing the socket: %s\n", strerror(errno));
		exit(1);
	}

	return 0;
}