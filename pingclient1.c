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

#define PORT_NUMBER 1234
#define BUFFER_SIZE 1500
#define OUTPUT_LENGTH 255 //got to check this

void check_arguments(int argc){
	if (argc != 2) {
		fprintf(stderr,"Usage: pingclient1 <destination address/name>\n");
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

void listen_port(int fd) {
	
	int err;
	char buff[BUFFER_SIZE];
	char msg[BUFFER_SIZE];
	socklen_t flen;
	struct sockaddr_in from;

	flen = sizeof(struct sockaddr_in);
	err = recvfrom(fd, buff, BUFFER_SIZE, 0, (struct sockaddr *) &from, &flen);
	printf("%d\n", err);
	if (err < 0) {
		fprintf(stderr, "Erro while receiving the message: %s\n", strerror(errno));
	} else {
		snprintf(msg, OUTPUT_LENGTH, "Received %d btes from host %s on port %d: %s\n", err, inet_ntoa(from.sin_addr), ntohs(from.sin_port), buff);
		fprintf(stdout, "%s\n" , msg);
	}
}

void send_message(int fd, char *dest_addr){
	int err;
	char buff[BUFFER_SIZE];
	char msg[BUFFER_SIZE];
	struct sockaddr_in dest;

	dest.sin_family = AF_INET;
	dest.sin_port = htons(PORT_NUMBER);
	
	dest.sin_addr.s_addr = inet_addr(dest_addr);	

	err = sendto(fd, buff, BUFFER_SIZE, 0, (struct sockaddr*) &dest, sizeof(struct sockaddr_in));
	if ( err < 0) {
		fprintf(stderr, "Error while sending message: %s\n", strerror(errno));		
	} else {
		snprintf(msg, OUTPUT_LENGTH ,"Sent %d bytes to host %s port %d: %s", err, inet_ntoa(dest.sin_addr), ntohs(dest.sin_port), buff);
		fprintf(stdout, "%s\n", msg);
		listen_port(fd);
	}
}


int main(int argc, char** argv) {
	int socket_fd;

	check_arguments(argc);

	socket_fd = create_socket();

	bind_socket(socket_fd);

	send_message(socket_fd, (char *) argv[1]);

	return 0;
}