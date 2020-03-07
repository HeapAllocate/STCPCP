#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "../include/main.h"
#include "../include/list.h"
#include "../../shared/protocol.h"
#include "../../shared/config.h"
#include "../include/connection.h"

connection_s *connection_new(void *srv, int sock, struct sockaddr_in *addr)
{
    connection_s *client = (connection_s *)list_init_element(sizeof(connection_s));

    client->sock = sock;
    client->status = STATUS_LOGIN_PENDING;
    client->srv = srv;
    memcpy(&client->addr, addr, sizeof(struct sockaddr_in));
    client->data.tv = &((server_data_s *)srv)->tv;
    
    list_insert_at_end((void **)&((server_data_s *)srv)->connection, client);
    
    return client;
}

connection_s *connection_delete(connection_s *conn)
{
    close(conn->sock);
    socket_reset(&conn->data);

    if (conn->status == STATUS_LOGIN_OK)
    {
        printf("Connection terminated: %s:%d\n", inet_ntoa(conn->addr.sin_addr),
                                                 ntohs(conn->addr.sin_port));
        ((server_data_s *)conn->srv)->num_connections--; //decrement current amount of connections
    }

    return (connection_s *)list_find_remove((void **)&((server_data_s *)conn->srv)->connection, conn);
}

int connection_authenticate(connection_s *conn) 
{   
    int r;   

    r = socket_recv(conn->sock, &conn->data);   
    if (r == CONNECTION_DEAD)   
        return 0;   
    else if (r == CONNECTION_AVAIL)   
    {   
        if (!memcmp(conn->data.memd.ptr, USER_CODE, conn->data.memd.len))   
        {    
            printf("User connected: %s:%d\n", inet_ntoa(conn->addr.sin_addr), 
                                              ntohs(conn->addr.sin_port));   
            conn->permission = PERMISSION_USER;    
            r = 1;   
        }   
        else if (!memcmp(conn->data.memd.ptr, ADMIN_CODE, conn->data.memd.len))   
        {   
            printf("Admin connected: %s:%d\n", inet_ntoa(conn->addr.sin_addr),
                                               ntohs(conn->addr.sin_port));   
            conn->permission = PERMISSION_ADMIN;   
            r = 1;   
        }   
        else   
            r = 0;   
   
        socket_reset(&conn->data);   
        conn->status = STATUS_LOGIN_OK;   
        ((server_data_s *)conn->srv)->num_connections++;   
   
        return r;   
    }   
    else   
        return 1;   
} 

void connection_parse(connection_s *conn)
{
    if (*conn->data.memd.ptr == MSG_MESSAGE)
    {
        //send global message
        return;
    }
    else if (*conn->data.memd.ptr == MSG_COMMAND &&
             conn->permission == PERMISSION_ADMIN)
    {
        //issue global, or direct command
        return;
    }

    return;
}

void connection_process(void *srv)   
{   
    connection_s *conn;   
    int r;   
   
    //check for pending login
    conn = ((server_data_s *)srv)->connection;   
    while (conn)   
    {   
        if (conn->status == STATUS_LOGIN_PENDING)   
        {   
            //process authentication 
            if (!connection_authenticate(conn))   
            {   
                //invalid auth, terminate, and delete node
                conn = connection_delete(conn);   
                continue;   
            }   
        }   
        else   
        {   
            //try to recieive any data 
            r = socket_recv(conn->sock, &conn->data);   
            if (r == CONNECTION_DEAD)   
            {      
                conn = connection_delete(conn);   
                continue;   
            }   
            else if (r == CONNECTION_DEAD)   
            {   
                //parse data and act accordingly
                connection_parse(conn);   
                connection_reset(&conn->data);   
            }   
        }   
        conn = conn->next;   
    }   
   
    return;   
}  

void connection_reset(void *srv)
{
    connection_s *conn;

    conn = ((server_data_s *)srv)->connection;
    while (conn)
        conn = connection_delete(conn);
}