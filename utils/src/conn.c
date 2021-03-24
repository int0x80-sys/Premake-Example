#include "conn.h"

#ifndef __USE_XOPEN2K
    #define __USE_XOPEN2K
#endif

#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

bool conn_init(connection_t* conn, const char* host, const char* port)
{
    struct addrinfo* server_info;
    struct addrinfo* it;
    struct addrinfo  hints = { 0 };

    int fd;
    int getaddr_err;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddr_err = getaddrinfo(host, port, &hints, &server_info);

    if(getaddr_err)
    {
        fprintf(stderr, "getaddr error: %s\n", gai_strerror(getaddr_err));
        return false;
    }

    for(it = server_info; it != NULL; it = it->ai_next)
    {
        fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if(fd == -1)
            continue;

        if(connect(fd, it->ai_addr, it->ai_addrlen) == -1)
            continue;

        break;
    }

    freeaddrinfo(server_info);

    if(it == NULL)
    {
        perror("connect error: ");
        return false;        
    }

    conn->_fd = fd;
    conn->_host = strdup(host);
    conn->_port = strdup(port);

    return true;
}

void conn_close(connection_t* conn)
{
    shutdown(conn->_fd, 2);

    if(conn->_host) { free(conn->_host); conn->_host = NULL; }
    if(conn->_port) { free(conn->_port); conn->_port = NULL; }
}

bool conn_send(connection_t* conn, const void* data, uint64_t size)
{
    int64_t bytes_to_send = size;
    int64_t bytes_sended = 0;

    do
    {
        bytes_sended = send(conn->_fd, data + bytes_sended, bytes_to_send, MSG_NOSIGNAL);

        if(bytes_sended == -1)
        {
            if(errno == EPIPE)
                fprintf(stderr, "connection lost\n");
            else
                perror("send error: ");

            return false;
        }

        bytes_to_send -= bytes_sended;
    } while(bytes_to_send > 0);

    return true;
}

bool conn_recv(connection_t* conn, void* data, uint64_t size)
{
    int64_t bytes = recv(conn->_fd, data, size, 0);
    if(bytes <= 0)
    {
        if(bytes == 0)
            fprintf(stderr, "connection lost\n");
        else
            perror("recv error: ");

        return false;
    }

    return true;
}