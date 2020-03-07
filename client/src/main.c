#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include "../../shared/config.h"
#include "../../shared/protocol.h"

typedef struct _condata_s
{
	int sock;
	indata_s data;
	int connected;
} condata_s;

void parse(condata_s *cd, char *msg, int len)
{
	if (*msg == MSG_MESSAGE)
	{
		printf("Received message: %.*s\n", len -1, msg + 1);
		socket_send(cd->sock, msg + 1, len - 1, *msg);
	}

	return;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s [server] [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in addr;
	condata_s cd;
	int r;

	memset(&cd, 0, sizeof(condata_s));

	cd.sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cd.sock < 0)
	{
		fprintf(stderr, "Error: failed to initalize socket (%s)\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	//TODO resolve DNS to IP address, and command-line argument
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port        = htons(atoi(argv[2]));
	addr.sin_family      = AF_INET;

	if (connect(cd.sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "Error: failed to connect (%s)\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	cd.connected = 1;

	if (fcntl(cd.sock, F_SETFL, O_NONBLOCK) == -1)
	{
		fprintf(stderr, "Error: failed to set sock non-blocking (%s)\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	socket_send(cd.sock, USER_CODE, sizeof(USER_CODE) - 1, 0);
	printf("Connected to %s:%s\n", argv[1], argv[2]);

	while (cd.connected)
	{
		r = socket_recv(cd.sock, &cd.data);
		if (r == CONNECTION_AVAIL)
		{
			parse(&cd, cd.data.memd.ptr, cd.data.memd.len);
			socket_reset(&cd.data);
		}
		else if (r == CONNECTION_DEAD)
		{
			cd.connected = 0;
			break;
		}
	}

	printf("Connection terminated\n");
	close(cd.sock);
	socket_reset(&cd.data);
	
	exit(EXIT_SUCCESS);
}