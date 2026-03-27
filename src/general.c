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
    char third_word[200] = {0};

    if (sscanf(message, "%49s %49s %199[^\n]", first_word, second_word, third_word) >= 1)
    {
        if ((strcmp(first_word, "join") == 0) || (strcmp(first_word, "j") == 0))
        {
            printf("Processing 'join' command...\n");
            join(node, message);
        }
        if ((strcmp(first_word, "announce") == 0) || (strcmp(first_word, "a") == 0))
        {
            printf("Processing 'announce' command...\n");
            announce(node);
        }
        else if ((strcmp(first_word, "show") == 0) || (strcmp(first_word, "n") == 0) || (strcmp(first_word, "sg") == 0) || (strcmp(first_word, "sr") == 0))
        {
            if ((strcmp(second_word, "nodes") == 0) || (strcmp(first_word, "n") == 0))
            {
                show_nodes(node, message);
            }
            else if (strcmp(second_word, "neighbors") == 0 || (strcmp(first_word, "sg") == 0))
            {
                show_neighbors(node);
            }
            else if (strcmp(second_word, "routing") == 0 || (strcmp(first_word, "sr") == 0))
            {
                show_routing(node, message);
            }
        }
        else if ((strcmp(first_word, "leave") == 0) || (strcmp(first_word, "l") == 0))
        {
            close_tcp_connection(node->tcp_connected_socket);
            node->tcp_connected_socket = -1;
            close_tcp_connection(node->tcp_listening_socket);
            node->tcp_listening_socket = -1;
            for (int i = 0; i < 100; i++)
            {
                node->dist[i] = -1;
                node->succ[i] = -1;
                node->state[i] = 1; // passar para estado de coordenacao
            }
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

                close_tcp_connection(node->tcp_connected_socket);
                node->tcp_connected_socket = -1;
                close_tcp_connection(node->tcp_listening_socket);
                node->tcp_listening_socket = -1;
                for (int i = 0; i < 100; i++)
                {
                    node->dist[i] = -1;
                    node->succ[i] = -1;
                    node->state[i] = 1; // passar para estado de coordenacao
                }
                int neighbors_seen = 0;
                for (int i = 0; i < 100 && neighbors_seen <= node->n_neighbors; i++)
                {
                    if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
                    {
                        neighbors_seen++;
                        // eliminar vizinho do protocolo de encaminhamento
                        node->dist[i] = -1;
                        node->succ[i] = -1;
                        node->state[i] = 1; // passar para estado de coordenacao
                        remove_edge(node, i);
                    }
                }
                leave(node);
            }
            if (node->left)
            {
                node->exiting = true;
            }
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
                // remover do encaminhamento e depois fechar a conexão TCP
                node->dist[atoi(id)] = -1;
                node->succ[atoi(id)] = -1;
                node->state[atoi(id)] = 1; // passar para estado de coordenacao
                remove_edge(node, atoi(id));
            }
        }
        else if ((strcmp(first_word, "message") == 0) || (strcmp(first_word, "m") == 0))
        {
            if (strlen(third_word) > 129)
            {
                printf("Error: Message content exceeds maximum length of 129 characters.\n");
                return;
            }
            else
            {
                printf("Processing 'message' command...\n");
                message_function(node, message);
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
            else if (strcmp(first_word, "ROUTE") == 0)
            {
                route(node, received_message, id);
            }
            else if (strcmp(first_word, "CHAT") == 0)
            {
                chat(node, received_message, id);
            }
            else if (strcmp(first_word, "COORD") == 0)
            {
                coord(node, id, received_message);
            }
            else if (strcmp(first_word, "UNCOORD") == 0)
            {
                uncoord(node, id, received_message);
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
    else // se retornar NULL a conexão foi fechada
    {
        if (node->receiving_neighbor) // caso estivesse no processo ainda de receber o vizinho
        {
            node->receiving_neighbor = false;
            node->tcp_connected_socket = -1;
        }
        else // caso ja fosse um vizinho estabelecido
        {

            // eliminar como vizinho
            node->n_neighbors--;
            node->is_a_connected_neighbor[id] = false;
            free_neighbor(node->neighbors[id]);
            node->neighbors[id] = NULL;

            coordenation_after_loss(node, id);
        }
    }
}
