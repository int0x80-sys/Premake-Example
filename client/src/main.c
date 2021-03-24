#include <utils.h>
#include <stdio.h>
#include <string.h>

#define CONN_SEND_AND_CHECK(conn, data, size)\
    if(!conn_send((conn), (data), (size))) return -1

#define CONN_RECV_AND_CHECK(conn, data, size)\
    if(!conn_recv((conn), (data), (size))) return -1

int main(void)
{
    char buffer[1024] = { 0 };
    connection_t conn = { 0 };

    if(!conn_init(&conn, "localhost", "3333"))
        return -1;


    CONN_SEND_AND_CHECK(&conn, "Hello From Client", strlen("Hello From Client") + 1);
    CONN_RECV_AND_CHECK(&conn, buffer, 1024);

    printf("Server response: %s\n", buffer);

    conn_close(&conn);

    return 0;
}