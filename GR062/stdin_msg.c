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
        //strcat(nodes_message, "\n");
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
       // strcat(nodes_message, "\n");
    }else if (sscanf(message, "%49s %49s", cmd, net) == 2)
    {

        strcat(nodes_message, "NODES ");
        strcat(nodes_message, node->tid);
        strcat(nodes_message, " 0 ");
        strcat(nodes_message, net);
        //strcat(nodes_message, "\n");
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
    //strcat(reg_message, "\n");
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
        //strcat(contact_message, "\n");
    }
    else if (sscanf(message, "%49s %49s", cmd, id) == 2)
    {
        strcat(contact_message, "CONTACT ");
        strcat(contact_message, node->tid);
        strcat(contact_message, " 0 ");
        strcat(contact_message, node->net);
        strcat(contact_message, " ");
        strcat(contact_message, id);
        //strcat(contact_message, "\n");
    }
    else
    {
        printf("Error: Invalid command format for add_edge.\n");
        return;
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

void announce(node_s *node)
{

    char route_message[256] = {0};
    strcat(route_message, "ROUTE ");
    strcat(route_message, node->id);
    strcat(route_message, " 0\n");

    int neighbors_seen = 0;
    for (int i = 0; i < 100 && neighbors_seen <= node->n_neighbors; i++)
    {
        if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
        {
            neighbors_seen++;

            send_message_tcp(node->neighbors[i]->tcp_socket, route_message);
        }
    }
}

void show_routing(node_s *node, const char *message)
{

    char cmd[50] = {0};
    char cmd2[50] = {0};
    char dest[50] = {0};

    if (sscanf(message, "%49s %49s %49s", cmd, cmd2, dest) == 3)
    {
        printf("Routing table for node %d:\n", atoi(dest));
        printf("Destination\tState\n");

        printf("%d\t\t%d\n", atoi(dest), node->state[atoi(dest)]);
        if (node->state[atoi(dest)] == 0)
        {
            printf("Distance: %d, Successor: %d\n", node->dist[atoi(dest)], node->succ[atoi(dest)]);
        }
    }
    else if (sscanf(message, "%49s %49s", cmd, dest) == 2)
    {
        printf("Routing table for node %d:\n", atoi(dest));
        printf("Destination\tState\n");

        printf("%d\t\t%d\n", atoi(dest), node->state[atoi(dest)]);
        if (node->state[atoi(dest)] == 0)
        {
            printf("Distance: %d, Successor: %d\n", node->dist[atoi(dest)], node->succ[atoi(dest)]);
        }
    }
}

void message_function(node_s *node, const char *message)
{
    char cmd[50] = {0};
    char dest[50] = {0};
    char msg_content[200] = {0};

    // O nó origem origin envia ao nó destino dest a mensagem chat. A sequência
    // de caracteres chat tem no máximo 128 caracteres.

    if (sscanf(message, "%49s %49s %199[^\n]", cmd, dest, msg_content) == 3)
    {
        // printf("Processing 'message' command 1...\n");
        int dest_id = atoi(dest);
        if ((dest_id < 0 || dest_id >= 100 || node->state[dest_id] == 0) && dest_id != atoi(node->id) && node->succ[dest_id] == -1)
        {
            printf("Error: Invalid destination ID.\n");
            return;
        }
        else if (node->state[dest_id] == 0)
        {
            char chat_message[256] = {0};
            strcat(chat_message, "CHAT ");
            strcat(chat_message, node->id);
            strcat(chat_message, " ");
            strcat(chat_message, dest);
            strcat(chat_message, " ");
            strcat(chat_message, msg_content);
            strcat(chat_message, "\n");
            send_message_tcp(node->neighbors[node->succ[dest_id]]->tcp_socket, chat_message);
            return;
        }
    }
}

void direct_join(node_s *node, const char *message)
{
    char cmd[50] = {0};
    char cmd2[50] = {0};
    char net[50] = {0};
    char id[50] = {0};

    if (sscanf(message, "%49s %49s %49s %49s", cmd, cmd2, net, id) == 4)
    {
        // store id and net in node structure
        strcpy(node->id, id);
        strcpy(node->net, net);

        node->joined = true;
    }
    else if (sscanf(message, "%49s %49s %49s", cmd, net, id) == 3)
    {
        // store id and net in node structure
        strcpy(node->id, id);
        strcpy(node->net, net);

        node->joined = true;
    }
}

void direct_add_edge(node_s *node, const char *message)
{
    char cmd[50] = {0};
    char cmd2[50] = {0};
    char cmd3[50] = {0};
    char id[50] = {0};
    char ip[50] = {0};
    char port[50] = {0};

    if (sscanf(message, "%49s %49s %49s %49s %49s %49s", cmd, cmd2, cmd3, id, ip, port) == 6)
    {
        char neighbor_message[256] = {0};
        strcat(neighbor_message, "NEIGHBOR ");
        strcat(neighbor_message, node->id);
        strcat(neighbor_message, "\n");

        int neighbor_idx = atoi(id);
        node->n_neighbors++;
        node->neighbors[neighbor_idx] = create_neighbor(id, ip, port);
        node->neighbors[neighbor_idx]->tcp_socket = connect_and_send_tcp_client(ip, port, neighbor_message);
        node->is_a_connected_neighbor[neighbor_idx] = true;
        node->adding_edge = false;
    }
    else if (sscanf(message, "%49s %49s %49s %49s", cmd, id, ip, port) == 4)
    {
        char neighbor_message[256] = {0};
        strcat(neighbor_message, "NEIGHBOR ");
        strcat(neighbor_message, node->id);
        strcat(neighbor_message, "\n");

        int neighbor_idx = atoi(id);
        node->n_neighbors++;
        node->neighbors[neighbor_idx] = create_neighbor(id, ip, port);
        node->neighbors[neighbor_idx]->tcp_socket = connect_and_send_tcp_client(ip, port, neighbor_message);
        node->is_a_connected_neighbor[neighbor_idx] = true;
        node->adding_edge = false;
    }
}