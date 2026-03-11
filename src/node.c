#include "node.h"
#include "udp.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Create and allocate the node structure
node_s *create_node(char *ip, char *port, char *regIP, char *regUDP)
{
    node_s *node = (node_s *)malloc(sizeof(node_s));
    if (node == NULL)
    {
        fprintf(stderr, "Error allocating memory for node\n");
        return NULL;
    }
    node->ip = ip;
    node->port = port;
    node->regIP = regIP;
    node->regUDP = regUDP;
    node->udp_socket = create_udp_socket();
    node->tcp_listening_socket = setup_tcp_server(port);
    node->tcp_connected_socket = -1; // Initialize to an invalid socket

    node->joining = false;
    node->joined = false;
    node->leaving = false;
    node->left = false;
    node->exiting = false;
    node->adding_edge = false;
    node->receiving_neighbor = false;

    srand(time(NULL));
    int numero = rand() % 1000;
    sprintf(node->tid, "%03d", numero);

    // neighbors
    node->n_neighbors = 0;
    node->is_a_connected_neighbor = (bool *)malloc(100 * sizeof(bool));
    if (node->is_a_connected_neighbor == NULL)
    {
        fprintf(stderr, "Error allocating memory for is_a_connected_neighbor\n");
        return NULL;
    }
    for (int i = 0; i < 100; i++)
    {
        node->is_a_connected_neighbor[i] = false;
    }

    node->neighbors = (neighbor_s **)malloc(100 * sizeof(neighbor_s *));
    if (node->neighbors == NULL)
    {
        fprintf(stderr, "Error allocating memory for neighbors\n");
        return NULL;
    }
    for (int i = 0; i < 100; i++)
    {
        node->neighbors[i] = NULL;
    }

    return node;
}

// free node structure
void free_node(node_s *node)
{
    if (node != NULL)
    {
        free(node);
    }
}

neighbor_s *create_neighbor(const char *id, const char *ip, const char *port)
{
    neighbor_s *neighbor = (neighbor_s *)malloc(sizeof(neighbor_s));
    if (neighbor == NULL)
    {
        fprintf(stderr, "Error allocating memory for neighbor\n");
        return NULL;
    }
    
    if (id != NULL)
    {
        strncpy(neighbor->id, id, sizeof(neighbor->id) - 1);
        neighbor->id[sizeof(neighbor->id) - 1] = '\0';
    }

    if (ip != NULL)
    {
        neighbor->ip = malloc(strlen(ip) + 1);
        if (neighbor->ip == NULL)
        {
            fprintf(stderr, "Error allocating memory for neighbor IP\n");
            free(neighbor);
            return NULL;
        }
        strcpy(neighbor->ip, ip);
    }

    if (port != NULL)
    {
        neighbor->port = malloc(strlen(port) + 1);
        if (neighbor->port == NULL)
        {
            fprintf(stderr, "Error allocating memory for neighbor port\n");
            free(neighbor->ip);
            free(neighbor);
            return NULL;
        }
        strcpy(neighbor->port, port);
    }

    neighbor->tcp_socket = -1; // Initialize to an invalid socket

    return neighbor;
}

void free_neighbor(neighbor_s *neighbor)
{
    if (neighbor != NULL)
    {
        free(neighbor->ip);
        free(neighbor->port);
        free(neighbor);
    }
}