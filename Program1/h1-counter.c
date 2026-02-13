// Group Members: Nicolas McDonald, Elijah Beverley
// EECE446
// Spring 2026

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

int lookup_and_connect( const char *host, const char *service );

// From Beej's Guide Chapter 7.4
int sendall(int s, char *buf, int *len)
{
	int total = 0;
	int bytes_left = *len;
	int n;

	while (total < *len)
	{
		n = send(s, buf+total, bytes_left, 0);
		if (n == -1)
		{
			perror("Error while sending request");
			break;	
		}
		total += n;
		bytes_left -= n;
	}
	*len = total;
	return n == -1?-1:0;
}

int main( int argc, char *argv[] ) {
	int s;

	if (argc != 2)
	{
		fprintf(stderr, "Wrong number of command line arguments given");	
		exit(1);
	}

	int chunk_size = atoi(argv[1]);
	char buf[chunk_size];

	if ((chunk_size < 5) || (chunk_size > 1000))
	{
		fprintf(stderr, "Chunk size must be a value between 5-999");
		exit(1);
	}

	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect( HOST, PORT ) ) < 0 ) {
		exit(1);
	}

	// Send HTTP request to the server
	char *http_req = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
	int req_len = (int)strlen(http_req);
	sendall(s, http_req, &req_len);

	//Keep recieving data from the server and write it until server stops sending data
	ssize_t bytes_recieved = 0;
	size_t total_bytes_recieved = 0;
	int tags_found = 0;
	
	while((bytes_recieved = recv(s, buf, chunk_size, 0)) > 0){
		if (bytes_recieved == -1)
		{
			perror("Error while receiving bytes");
			break;
		}

		// write code to find tags here
		total_bytes_recieved += (size_t)bytes_recieved;
	}

	close(s);
	return 0;
}

int lookup_and_connect( const char *host, const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Translate host name into peer's IP address */
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
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

