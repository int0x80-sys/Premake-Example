#define __USE_XOPEN2K

#include <sys/socket.h>
#include <netdb.h>

#include <utils.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CONN_SEND_AND_CHECK(conn, data, size)\
    if(!conn_send((conn), (data), (size))) { conn_close(conn); continue; }

#define CONN_RECV_AND_CHECK(conn, data, size)\
    if(!conn_recv((conn), (data), (size))) { conn_close(conn); continue; }

int32_t create_server(const char* port, int backlogs)
{
    struct addrinfo* server_info;
    struct addrinfo* it;
    struct addrinfo  hints = { 0 };

    int fd;
    int getaddr_err;

    int yes = 1;

    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    getaddr_err = getaddrinfo(NULL, port, &hints, &server_info);

    if(getaddr_err)
    {
        fprintf(stderr, "getaddr error: %s\n", gai_strerror(getaddr_err));
        return -1;
    }

    for(it = server_info; it != NULL; it = it->ai_next)
    {
        fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if(fd == -1)
            continue;

        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if(bind(fd, it->ai_addr, it->ai_addrlen) == -1)
            continue;

        break;
    }

    freeaddrinfo(server_info);

    if(it == NULL)
    {
        perror("bind error: ");
        return -1;
    }

    if(listen(fd, backlogs) == -1)
    {
        perror("listen error: ");
        shutdown(fd, 2);

        return -1;
    }

    return fd;
}

int main(void)
{
    char buffer[1024] = { 0 };
    connection_t client_conn = { 0 };
    int32_t server_socket;

    server_socket = create_server("3333", 128);
    if(server_socket == -1)
        return -1;

    while(1)
    {
        client_conn._fd = accept(server_socket, NULL, NULL);

        if(client_conn._fd == -1)
        {
            perror("accept error: ");
            continue;
        }

        CONN_RECV_AND_CHECK(&client_conn, buffer, 1024);
        CONN_SEND_AND_CHECK(&client_conn, "Hello From Server", strlen("Hello From Server") + 1);

        conn_close(&client_conn);

        printf("Client send new message: %s\n", buffer);
    }
}