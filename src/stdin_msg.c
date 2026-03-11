#include "node.h"
#include "udp.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void join(node_s *node, const char *message)
{
    node->joining = true;

    char cmd[50] = {0};
    char nodes_message[256] = {0};

    if (sscanf(message, "%49s %49s %49s", cmd, node->net, node->id) == 3)
    {

        strcat(nodes_message, "NODES ");
        strcat(nodes_message, node->tid);
        strcat(nodes_message, " 0 ");
        strcat(nodes_message, node->net);
        strcat(nodes_message, "\n");
    }

    // store id and net in node structure

    // send nodes

    send_udp_message(node->udp_socket, node->regIP, node->regUDP, nodes_message);
}

void show_nodes(node_s *node, const char *message)
{

    char cmd[50] = {0};
    char cmd2[50] = {0};
    char net[50] = {0};
    char nodes_message[256] = {0};

    if (sscanf(message, "%49s %49s %49s", cmd, cmd2, net) == 3)
    {

        strcat(nodes_message, "NODES ");
        strcat(nodes_message, node->tid);
        strcat(nodes_message, " 0 ");
        strcat(nodes_message, net);
        strcat(nodes_message, "\n");
    }

    // send nodes
    send_udp_message(node->udp_socket, node->regIP, node->regUDP, nodes_message);
}

void leave(node_s *node)
{
    node->leaving = true;
    char reg_message[256] = {0};
    strcat(reg_message, "REG ");
    strcat(reg_message, node->tid);
    strcat(reg_message, " 3 ");
    strcat(reg_message, node->net);
    strcat(reg_message, "  ");
    strcat(reg_message, node->id);
    strcat(reg_message, "\n");
    send_udp_message(node->udp_socket, node->regIP, node->regUDP, reg_message);
}

void add_edge(node_s *node, const char *message)
{

    char cmd[50] = {0};
    char cmd2[50] = {0};
    char id[50] = {0};
    char contact_message[256] = {0};

    if (sscanf(message, "%49s %49s %49s", cmd, cmd2, id) == 3)
    {

        strcat(contact_message, "CONTACT ");
        strcat(contact_message, node->tid);
        strcat(contact_message, " 0 ");
        strcat(contact_message, node->net);
        strcat(contact_message, " ");
        strcat(contact_message, id);
        strcat(contact_message, "\n");
    }

    node->adding_edge = true;

    // send contact message
    send_udp_message(node->udp_socket, node->regIP, node->regUDP, contact_message);
}

void show_neighbors(node_s *node)
{
    printf("Node %s neighbors are:\n", node->id);
    int neighbors_seen = 0;
    for (int i = 0; i < 100 && neighbors_seen <= node->n_neighbors; i++)
    {
        if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
        {
            neighbors_seen++;
            printf("Neighbor %d ID: %s, IP: %s, Port: %s\n", neighbors_seen, node->neighbors[i]->id, node->neighbors[i]->ip, node->neighbors[i]->port);
        }
    }
}

void remove_edge(node_s *node, int id)
{

    close_tcp_connection(node->neighbors[id]->tcp_socket);
    free_neighbor(node->neighbors[id]);
    node->neighbors[id] = NULL;
    node->is_a_connected_neighbor[id] = false;
    node->n_neighbors--;
}