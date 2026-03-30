#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int setup_tcp_server(const char *port);
int new_tcp_connection(int listen_fd);
char * receive_message_tcp(int connected_fd, char *buffer, size_t buffer_size);
void send_message_tcp(int connected_fd, const char *message);
int connect_and_send_tcp_client(const char *server_ip, const char *port, const char *message);
void close_tcp_connection(int connected_fd);