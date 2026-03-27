#include "node.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void neighbor(node_s *node, const char *received_message);
void route(node_s *node, const char *received_message, int id);
void chat(node_s *node, const char *received_message, int id);
void coordenation_after_loss(node_s *node, int id);
void coord(node_s *node, int id, char *received_message);
void uncoord(node_s *node, int id, char *received_message);