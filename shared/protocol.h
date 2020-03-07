#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/time.h>

typedef struct _data_s
{
    char *ptr;          //pointer to allocated data
    unsigned short len; //length of allocated data
} data_s;

typedef struct _indata_s
{
    data_s memd;          //memory for data
    unsigned short d_len; //total length of data being received
    char b_len[2];        //buffer for saving length from incomplete data stream
    int b_len_len;        //length of data in b_len
    struct timeval *tv;
} indata_s;

#define CONNECTION_DEAD -1
#define CONNECTION_AVAIL 1
#define CONNECTION_NN    0 //nothing new

void socket_send(int sock, char *data, unsigned short length, char code);
int socket_recv(int sock, indata_s *data);
void socket_reset(indata_s *data);

#endif
