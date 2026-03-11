#include "node.h"
#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void join(node_s *node, const char *message);
void show_nodes(node_s *node, const char *message);
void leave(node_s *node);
void add_edge(node_s *node, const char *message);
void show_neighbors(node_s *node);
void remove_edge(node_s *node, int id);