#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int create_udp_socket(void);
void send_udp_message(int socketfd, const char *ip, const char *port, const char *message);
char * receive_udp_message(int socketfd, char *buffer, size_t buffer_size);