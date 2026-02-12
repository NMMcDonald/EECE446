#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
  int in_fd;
  int out_fd;
  char filetext[1000];
  int bytes_read;
  int bytes_written;
	
	in_fd = open(argv[1], O_RDONLY);
	if (in_fd == -1)
	{
		perror("Could not open file");
		exit(1);
	}
	out_fd = open("upper_file", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (out_fd == -1)
	{
		perror("Output file could not be opened");
		exit(1);
	}
	while((bytes_read = read(in_fd, filetext, sizeof(filetext))) > 0)
	{
		if (bytes_read == -1)
		{
			perror("Error reading input file");
			exit(1);
		}

		write(1, filetext, bytes_read); // writes to STDOUT the bytes read from read()
		
		for (int i = 0; i < bytes_read; i++)
		{
			filetext[i] = toupper(filetext[i]);
		}
		
		bytes_written = write(out_fd, filetext, bytes_read);
		if (bytes_written == -1)
		{
			perror("Error while writing to output file");
			exit(1);
		}
	}
	
	close(in_fd);
	close(out_fd);
	return 0;
}
