#include "node.h"
#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void nodes(char *received_message, node_s *node);
void reg(node_s *node, const char *received_message); 
void contact(node_s *node, const char *received_message);