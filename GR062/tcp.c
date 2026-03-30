#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int setup_tcp_server(const char *port)
{
    struct addrinfo hints, *res;
    int listen_fd, errcode;

    // 1. Prepare hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // AI_PASSIVE tells getaddrinfo we intend to use this for a listening server

    errcode = getaddrinfo(NULL, port, &hints, &res);
    if (errcode != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errcode));
        exit(EXIT_FAILURE);
    }

    // Create the socket
    listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listen_fd < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // bind claim the port on this computer
    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0)
    {
        perror("Error binding to port");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);

    // Listen tells the OS to start accepting incoming connection requests
    // The '5' is the backlog: how many pending connections the OS will queue
    // before turning people away while your code is busy processing.
    if (listen(listen_fd, 20) < 0)
    {
        perror("Error listening");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %s...\n", port);
    return listen_fd;
}

int new_tcp_connection(int listen_fd)
{

    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);

    if (client_fd < 0)
    {
        perror("Error: Failed to accept new connection");
        return -1;
    }

    // char client_ip[256];
    // char client_port[256];

    // getnameinfo((struct sockaddr *)&client_addr, addr_len, client_ip, sizeof(client_ip), client_port, sizeof(client_port), 256 | 256);

    // printf("New TCP connection established with [%s]:%s on FD %d\n", client_ip, client_port, client_fd);

    return client_fd;
}

void send_message_tcp(int connected_fd, const char *message)
{
    size_t msg_len = strlen(message);

    ssize_t bytes_sent = send(connected_fd, message, msg_len, 0);

    if (bytes_sent < 0)
    {
        perror("Error: Failed to send TCP message");
    }
    else
    {
        //printf("Sent %s with %zd bytes\n", message, bytes_sent);
    }
}

char *receive_message_tcp(int connected_fd, char *buffer, size_t buffer_size)
{

    ssize_t bytes_read = recv(connected_fd, buffer, buffer_size - 1, 0);

    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        //printf("Received %s with %zd bytes\n", buffer, bytes_read);
    }
    else if (bytes_read == 0)
    {
        printf("TCP connection closed by peer on FD %d\n", connected_fd);
        close(connected_fd);
        return NULL; // Return NULL so general.c knows to delete the neighbor
    }
    else
    {
        // A return value < 0 means a network error occurred.
        perror("Error: Failed to receive TCP message");
    }
    return buffer;
}

int connect_and_send_tcp_client(const char *server_ip, const char *port, const char *message)
{
    struct addrinfo hints, *res, *p;
    int sockfd, errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo(server_ip, port, &hints, &res);
    if (errcode != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errcode));
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            continue; // If socket creation fails, try the next address
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0)
        {
            break;
        }
        close(sockfd);
    }
    freeaddrinfo(res);

    if (p == NULL)
    {
        fprintf(stderr, "Error: Failed to connect to %s:%s\n", server_ip, port);
        return -1;
    }

    ssize_t bytes_sent = send(sockfd, message, strlen(message), 0);

    if (bytes_sent < 0)
    {
        perror("Error: Failed to send message to server");
        close(sockfd);
        return -1;
    }

    //printf("Successfully connected and sent %zd bytes to %s %s\n", bytes_sent, server_ip, port);

    return sockfd;
}

void close_tcp_connection(int connected_fd)
{
    close(connected_fd);
}
