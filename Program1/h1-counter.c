/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

 #define HOST "www.ecst.csuchico.edu"
 #define PORT "80"
 #define REQUEST "GET /~kkredo/file.html HTTP/1.0\r\n\r\n"
 #define MAX_BUF_SIZE 256

/*
 * Lookup a host IP address and connect to it using service. Arguments match the first two
 * arguments to getaddrinfo(3).
 *
 * Returns a connected socket descriptor or -1 on error. Caller is responsible for closing
 * the returned socket.
 */
int lookup_and_connect( const char *host, const char *service );



int main( int argc, char *argv[] ) {
	char buf[MAX_BUF_SIZE];
	int s;
	int outfd;
	char* fail_msg= "Failed to open output file\n";
	if ((outfd = open("local_file", 0_WRONLY)) == -1){
		write(STDOUT_FILENO, fail_msg, strlen(fail_msg));
	}

	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect( HOST, PORT ) ) < 0 ) {
		exit( 1 );
	}

	// Send HTTP request to the server
	char* http_req = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
	send(s,http_req, strlen(http_req), 0);

	//Keep recieving data from the server and write it until server stops sending data
	ssize_t bytes_recieved = 0;
	size_t total_bytes_recieved = 0;
	
	while((bytes_recieved = recv(s, buf, 100, 0)) > 0){
		write(outfd, buf, (size_t)bytes_recieved);
		total_bytes_recieved += (size_t)bytes_recieved;
	}


	close( s );
	close(outfd);

	return 0;
}



int lookup_and_connect( const char *host, const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Translate host name into peer's IP address */
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ( ( s = getaddrinfo( host, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "stream-talk-client: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to connect */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
			continue;
		}

		if ( connect( s, rp->ai_addr, rp->ai_addrlen ) != -1 ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "stream-talk-client: connect" );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}

