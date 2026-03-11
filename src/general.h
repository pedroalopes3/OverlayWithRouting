#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "node.h"


void process_stdin_message(const char *message, node_s *node);
void process_udp_message(node_s *node);
void process_tcp_server_message(node_s *node);
void process_tcp_connection_message(node_s *node,int id);