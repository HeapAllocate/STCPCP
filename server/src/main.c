#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "../include/list.h"
#include "../include/main.h"
#include "../../shared/protocol.h"

/* TODO
    * Make multi-threaded to handle multiple clients at once.
*/

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [PORT]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int clientfd; //client sock
    server_data_s srv;
    struct sockaddr_in addr;
    int len = sizeof(struct sockaddr_in);

    srv.connection = NULL;
    srv.tv.tv_sec = 0;
    srv.tv.tv_usec = 0;
    srv.num_connections = 0;

    srv.sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (srv.sock  == -1)
    {
        fprintf(stderr, "Error: failed to initalize socket (%s)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(srv.sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
    {
        fprintf(stderr, "Error: failed to bind port on %s (%s)\n", argv[1], strerror(errno));
        close(srv.sock);
        exit(EXIT_FAILURE);
    }

    if (listen(srv.sock, SOMAXCONN) == -1)
    {
        fprintf(stderr, "Error: start listening on port %s (%s)\n", argv[1], strerror(errno));
        close(srv.sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %s\n", argv[1]);

    fcntl(srv.sock, F_SETFL, O_NONBLOCK);

    while (srv.sock != -1)
    {
        clientfd = accept(srv.sock, (struct sockaddr *)&addr, (socklen_t *)&len);
        if (clientfd > 0)
        {
            connection_new(&srv, clientfd, &addr);
        }

        connection_process(&srv);
    
    }

    close(srv.sock);

    exit(EXIT_SUCCESS);
}