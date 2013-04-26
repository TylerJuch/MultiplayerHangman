#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

char *sockreadline(int fd) {
	char *buf = (char *) malloc(BUFSIZ);
	int len = 0;
	char c;

	while (1) {

		if (read(fd, &c, 1) != 1) {
			return NULL;
		}

		buf[len] = c;
	
		switch(c) {
		case '\n':
		case '\r':
		case '\f':
			if (len == 0) continue;

			buf[len] = '\0';
			return buf;
		}

		len++;
		buf = (char *)realloc(buf, len);
	}
}