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
#define BUFFER_SIZE 64

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

void clear_buffer(char *buff) {
	int i;
	for(i = 0 ; i < BUFFER_SIZE; i++){
		buff[i] = '\0';
	}	
}

void send_response(int fd, char *buff, struct sockaddr_in dest){
	int err;	

	err = sendto(fd, buff, BUFFER_SIZE, 0, (struct sockaddr*) &dest, sizeof(struct sockaddr_in));
	if ( err < 0) {
		fprintf(stderr, "Error while sending response message: %s\n" , strerror(errno));		
	} else {
		err = printf("Sent %d bytes to host %s port %d: %s\n", err, inet_ntoa(dest.sin_addr), ntohs(dest.sin_port), buff);
		if (err < 0) {
			fprintf(stderr, "Error while writting output: %s\n", strerror(errno));
			exit(1);
		}
	}
}

void listen_port(int fd) {

	int err;
	char buff[BUFFER_SIZE];	
	socklen_t flen;
	struct sockaddr_in from;

	clear_buffer(buff);

	flen = sizeof(struct sockaddr_in);
	err = recvfrom( fd, buff, BUFFER_SIZE, 0, (struct sockaddr *) &from, &flen);		
	if (err < 0)	 {
		fprintf(stderr, "Error while receiving message: %s\n", strerror(errno));		
	} else {
		err = printf("Received %d bytes from host %s port %d: %s\n", err, inet_ntoa(from.sin_addr), ntohs(from.sin_port), buff);		
		if (err < 0) {
			fprintf(stderr, "Error while writting output: %s\n", strerror(errno));
			exit(1);
		}
		send_response(fd, (char *) &buff, from);
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