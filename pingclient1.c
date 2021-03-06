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
#include <sys/time.h>
#include <netdb.h>

#define PORT_NUMBER 1234
#define BUFFER_SIZE 64
#define USEC_PER_SEC 100000
#define MESSAGE "Echo"

void check_arguments(int argc){
	if (argc != 2) {
		fprintf(stderr,"Usage: pingclient1 <destination address/name>\n");
		exit(EXIT_FAILURE);
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

int listen_port(int fd) {	
	int err;	
	char buff[BUFFER_SIZE];	
	socklen_t flen;
	struct sockaddr_in from;
	
	clear_buffer(buff);

	flen = sizeof(struct sockaddr_in);	
	err = recvfrom(fd, buff, BUFFER_SIZE, 0, (struct sockaddr *) &from, &flen);	
	if (err < 0) {
		fprintf(stderr, "Error while receiving the message: %s\n", strerror(errno));
		return 0;
	} else {		
		return 1;				
	}
}

void send_message(int fd, char *dest_hostname){
	int err;
	long int init_time, fin_time;
	double rtt;
	char buff[BUFFER_SIZE] = MESSAGE;	
	struct sockaddr_in dest;
	struct hostent *he;
	struct timeval *tv1 = malloc(sizeof(struct timeval));
	struct timeval *tv2 = malloc(sizeof(struct timeval));
	struct timeval *tv_tot = malloc(sizeof(struct timeval));

	he = gethostbyname(dest_hostname);
	if( he == NULL) {
		fprintf(stderr, "The name '%s' could not be resolved.\n", dest_hostname);
		exit(1);		
	}
	
	dest.sin_family = AF_INET;
	dest.sin_port = htons(PORT_NUMBER);
	memcpy(&dest.sin_addr, he->h_addr_list[0], he->h_length);
	
	err = sendto(fd, buff, BUFFER_SIZE, 0, (struct sockaddr*) &dest, sizeof(struct sockaddr_in));
	if ( err < 0) {
		fprintf(stderr, "Error while sending message: %s\n", strerror(errno));		
	} else {
		init_time = gettimeofday(tv1, NULL);
		if (init_time < 0) {
			fprintf(stderr, "Error while setting the timer: %s\n", strerror(errno));		
		}				
		if(listen_port(fd)) {
			fin_time = gettimeofday(tv2, NULL);
			if (fin_time < 0) {
				fprintf(stderr, "Error while setting the timer: %s\n", strerror(errno));		
			}		
			timersub(tv2,tv1,tv_tot);			
			rtt = ((double) (tv_tot->tv_usec)) / USEC_PER_SEC;			
			err = printf("The RTT was: %f seconds.\n", rtt);
			if (err < 0) {
				fprintf(stderr, "Error while writting output: %s\n", strerror(errno));
				exit(1);
			}
		}
	}
	free(tv1);
	free(tv2);
	free(tv_tot);
}

int main(int argc, char** argv) {
	int socket_fd;

	check_arguments(argc);

	socket_fd = create_socket();

	bind_socket(socket_fd);

	send_message(socket_fd, (char *) argv[1]);

	return 0;
}