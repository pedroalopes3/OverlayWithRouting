#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int create_udp_socket(void)
{
    int sockfd;

    // AF_INET: IPv4 protocol
    // SOCK_DGRAM: Datagram-based socket (UDP)
    // 0: Default protocol for the chosen domain and type IJJOK
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Error: Failed to create UDP socket");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void send_udp_message(int socketfd, const char *ip, const char *port, const char *message)
{
    struct addrinfo hints, *res;
    int errcode;

    // Prepare the hints filter
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // AF_UNSPEC allows both IPv4 and IPv6!
    hints.ai_socktype = SOCK_DGRAM; // UDP

    // Fetch and format the address
    errcode = getaddrinfo(ip, port, &hints, &res);
    if (errcode != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errcode));
        exit(EXIT_FAILURE);
    }

    // Send the message
    ssize_t bytes_sent = sendto(socketfd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);

    if (bytes_sent < 0)
    {
        perror("Error: Failed to send message");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    // printf("Successfully sent %s with %zd bytes to %s:%s\n", message, bytes_sent, ip, port);

    // To prevent memory leaks
    freeaddrinfo(res);
}

char *receive_udp_message(int socketfd, char *buffer, size_t buffer_size)
{
    struct sockaddr_storage sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);

    ssize_t bytes_received = recvfrom(socketfd, buffer, buffer_size - 1, 0, (struct sockaddr *)&sender_addr, &sender_addr_len);

    if (bytes_received < 0)
    {
        perror("Error: Failed to receive message");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';

    //printf("Received %s with %zd bytes\n", buffer, bytes_received);

    return buffer;
}
