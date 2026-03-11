#include "general.h"
#include "udp_msg.h"
#include "tcp_msg.h"
#include "node.h"
#include "udp.h"
#include "stdin_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void process_stdin_message(const char *message, node_s *node)
{
    char first_word[50] = {0};
    char second_word[50] = {0};

    if (sscanf(message, "%49s %49s", first_word, second_word) >= 1)
    {
        if ((strcmp(first_word, "join") == 0) || (strcmp(first_word, "j") == 0))
        {
            printf("Processing 'join' command...\n");
            join(node, message);
        }
        else if ((strcmp(first_word, "show") == 0) || (strcmp(first_word, "n") == 0) || (strcmp(first_word, "sg") == 0) || (strcmp(first_word, "sr") == 0))
        {
            if ((strcmp(second_word, "nodes") == 0) || (strcmp(second_word, "n") == 0))
            {
                show_nodes(node, message);
            }
            else if (strcmp(second_word, "neighbors") == 0 || (strcmp(second_word, "sg") == 0))
            {
                show_neighbors(node);
            }
        }
        else if ((strcmp(first_word, "leave") == 0) || (strcmp(first_word, "l") == 0))
        {
            int neighbors_seen = 0;
            for (int i = 0; i < 100 && neighbors_seen <= node->n_neighbors; i++)
            {
                if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
                {
                    neighbors_seen++;
                    remove_edge(node, i);
                }
            }
            leave(node);
        }
        else if ((strcmp(first_word, "exit") == 0) || (strcmp(first_word, "x") == 0))
        {
            if (node->joined)
            {
                int neighbors_seen = 0;
                for (int i = 0; i < 100 && neighbors_seen <= node->n_neighbors; i++)
                {
                    if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
                    {
                        neighbors_seen++;
                        remove_edge(node,i);
                    }
                }
                leave(node);
            }
            node->exiting = true;
        }
        else if ((strcmp(first_word, "add") == 0) || (strcmp(first_word, "ae") == 0))
        {
            add_edge(node, message);
        }
        else if ((strcmp(first_word, "remove") == 0) || (strcmp(first_word, "re") == 0))
        {
            char cmd[50] = {0};
            char cmd2[50] = {0};
            char id[50] = {0};

            if (sscanf(message, "%49s %49s %49s", cmd, cmd2, id) == 3)
            {
                remove_edge(node, atoi(id));
            }
        }
        else
        {
            printf("Received unknown message type: %s\n", first_word);
        }
    }
}

void process_udp_message(node_s *node)
{
    char buffer[256];
    char *received_message = receive_udp_message(node->udp_socket, buffer, sizeof(buffer));
    if (received_message != NULL)
    {
        char first_word[50];

        if (sscanf(received_message, "%49s", first_word) == 1)
        {

            if (strcmp(first_word, "REG") == 0)
            {
                reg(node, received_message);
            }
            else if (strcmp(first_word, "NODES") == 0)
            {
                nodes(received_message, node);
            }
            else if (strcmp(first_word, "CONTACT") == 0)
            {
                contact(node, received_message);
            }
            else
            {
                printf("Received unknown message type: %s\n", first_word);
            }
        }
        else
        {
            printf("Error: Could not read any words from the string.\n");
        }
    }
}

void process_tcp_server_message(node_s *node)
{
    node->tcp_connected_socket = new_tcp_connection(node->tcp_listening_socket);
    node->receiving_neighbor = true;
}

void process_tcp_connection_message(node_s *node, int id)
{
    char buffer[256];
    char *received_message = NULL;
    if (id == -1)
    {
        received_message = receive_message_tcp(node->tcp_connected_socket, buffer, sizeof(buffer));
    }
    else
    {
        received_message = receive_message_tcp(node->neighbors[id]->tcp_socket, buffer, sizeof(buffer));
    }
    if (received_message != NULL)
    {
        char first_word[50];

        if (sscanf(received_message, "%49s", first_word) == 1)
        {

            if (strcmp(first_word, "NEIGHBOR") == 0)
            {
                neighbor(node, received_message);
            }
            else
            {
                printf("Received unknown message type: %s\n", first_word);
            }
        }
        else
        {
            printf("Error: Could not read any words from the string.\n");
        }
    }
    else
    {
        if (node->receiving_neighbor)
        {
            node->receiving_neighbor = false;
            node->tcp_connected_socket = -1;
        }
        else
        {
            node->n_neighbors--;
            node->is_a_connected_neighbor[id] = false;
            free_neighbor(node->neighbors[id]);
            node->neighbors[id] = NULL;
        }
    }
}
