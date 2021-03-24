#ifndef CONN_H
#define CONN_H

#include <stdint.h>
#include <stdbool.h>

typedef struct connection_t
{
    int32_t _fd;
    char*   _host;
    char*   _port;
} connection_t;

bool conn_init(connection_t* conn, const char* host, const char* port);
void conn_close(connection_t* conn);

bool conn_send(connection_t* conn, const void* data, uint64_t size);
bool conn_recv(connection_t* conn, void* data, uint64_t size);

#endif