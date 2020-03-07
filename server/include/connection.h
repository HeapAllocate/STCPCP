#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "list.h"
#include "../../shared/protocol.h"

#define STATUS_LOGIN_PENDING 0
#define STATUS_LOGIN_OK      1

typedef struct _connection_s
{
    LIST_MEMBERS(struct _connection_s);
    struct sockaddr_in addr;
    void *srv;              //pointer to server data
    int sock;               //sock for comms
    int status;             //current connection status
    int permission;         //permission level
    indata_s data;          //incoming data
} connection_s;

connection_s *connection_new(void *srv, int sock, struct sockaddr_in *addr);
connection_s *connection_delete(connection_s *connection);
void connection_process(void *srv);
void connection_reset(void *srv);

#endif