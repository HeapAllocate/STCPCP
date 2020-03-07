#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>

#include "protocol.h"

void socket_send(int sock, char *data, unsigned short length, char code)
{
    unsigned short sz;

    if (!data || !length)
        return;

    sz = length;
    if (code)
        sz++;

    //send length of data first (2 bytes)
    if (send(sock, (char *)&sz, 2, 0) == -1)
    {
        fprintf(stderr, "Error: failed to send length data (%s)\n", strerror(errno));
        return;
    }

    if (code)
    {
        if (send(sock, (char *)&code, 1, 0) == -1)
        {
            fprintf(stderr, "Error: failed to send code (%s)\n", strerror(errno));
            return;
        }
    }

    //send the actual data
    if (send(sock, data, length, 0) == -1)
    {
        fprintf(stderr, "Error: failed to send data (%s)\n", strerror(errno));
        return;
    }

    printf("Sent data: %s\n", data);

    return;
}

int socket_recv(int sock, indata_s *data)   
{   
    fd_set      fds;   
    int         r;   
   
    FD_ZERO(&fds);   
    FD_SET(sock, &fds);   

    //poll fd for any data
    if (select(0, &fds, NULL, NULL, data->tv) != 1)   
    {    
        //data waiting 
        if (data->d_len == 0)   
        {   
            if (data->b_len_len == 0)   
            {   
                //try to recieve length of data (2 bytes)
                r = recv(sock, data->b_len, 2, 0);   
            }   
            else   
            {   
                //try to receive length of data (1 byte?)
                r = recv(sock, &data->b_len[1], 1, 0);   
            }   
   
            if (r <= 0)   
                return CONNECTION_DEAD;     
   
            //check for atleast 2 bytes, then recv real data 
            data->b_len_len += r;   
            if (data->b_len_len == 2)   
            {   
                data->d_len = *(unsigned short *)data->b_len;   
                if (data->d_len == 0)   
                {   
                    //cant send data with length of 0
                    data->b_len_len = 0;   
                    return CONNECTION_NN;   
                }   

                //allocate size of incoming data
                data->memd.ptr = (char *)malloc(data->d_len);   
            }   
        }   
        else   
        {   
            //recv data into allocated memory
            r = recv(sock, data->memd.ptr + data->memd.len, data->d_len - data->memd.len, 0);   
   
            if (r <= 0)    
                return CONNECTION_DEAD;   

            data->memd.len += r;   

            if (data->memd.len == data->d_len)   
                return CONNECTION_AVAIL;   
        }   
    }  
   
    return CONNECTION_NN;   
}   

void socket_reset(indata_s *data)
{
    if (data->d_len > 0)
    {
        free(data->memd.ptr);
        data->memd.len = 0;
        data->d_len = 0;
    }
    data->b_len_len = 0;

    return;
}