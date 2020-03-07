#ifndef _MAIN_H_
#define _MAIN_H_

#include <sys/time.h>
#include "connection.h"

typedef struct _server_data_s
{
    connection_s *connection; //pointer to a linked-list of connections
    int sock;
    int num_connections;
    struct timeval tv;
} server_data_s;

#endif