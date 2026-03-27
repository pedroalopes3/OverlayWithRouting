#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include "node.h"
#include "general.h"

#define MAX_N_NET 999
#define MAX_N_ID 99

// struct timeval
// {
//     time_t tv_sec;       /* seconds */
//     suseconds_t tv_usec; /* microseconds */
// };

int calc_maxfd(node_s *node)
{
    int maxfd = 0;
    if (node->udp_socket > 0)
    {
        maxfd = node->udp_socket;

        if (node->tcp_listening_socket > maxfd)
        {
            maxfd = node->tcp_listening_socket;
        }

        if (node->tcp_connected_socket > maxfd)
        {
            maxfd = node->tcp_connected_socket;
        }

        int neighbors_seen = 0;
        for (int i = 0; i < 100 && neighbors_seen <= node->n_neighbors; i++)
        {
            if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
            {
                neighbors_seen++;
                if (node->neighbors[i]->tcp_socket > maxfd)
                {
                    maxfd = node->neighbors[i]->tcp_socket;
                }
            }
        }
    }
    return maxfd;
}


void node_state(node_s *node) // para debug no desenvolvimento
{
    printf("\n\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n                                    Node state:\n");
    printf("ID: %s, Network: %s\n", node->id, node->net);
    printf("Joining: %s, Joined: %s, Leaving: %s, Left: %s, Exiting: %s\n",
           node->joining ? "true" : "false",
           node->joined ? "true" : "false",
           node->leaving ? "true" : "false",
           node->left ? "true" : "false",
           node->exiting ? "true" : "false");
    printf("Number of neighbors: %d\n", node->n_neighbors);
    for (int i = 0; i < 100; i++)
    {
        if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
        {
            printf("Neighbor %d: ID: %s, IP: %s, Port: %s\n", i, node->neighbors[i]->id, node->neighbors[i]->ip, node->neighbors[i]->port);
        }
    }
    printf("\n    || Chat network status:\n");
    for (int i = 0; i < 100; i++)
    {
        if (node->dist[i] != -1)
        {
            printf("Node %d: dist: %d, succ: %d, state: %d, succ_coord: %d\n", i, node->dist[i], node->succ[i], node->state[i], node->succ_coord[i]);
        }
    }
    printf("\n\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n\n\n");
}

int main(int argc, char *argv[])
{

    char *IP;
    // Meu pc
    // 192.168.55.34
    char *TCP;
    char *regIP;
    char *regUDP;

    printf("\n|||||||||||||||||||||||||||||||||||||||||||||");
    printf("\n||||        OverlayWithRouting           ||||");
    printf("\n|||||||||||||||||||||||||||||||||||||||||||||");
    printf("\n\n       usage: OWR IP TCP regIP regUDP      \n\n");

    if (argc < 3)
    {
        printf("\n Please provide all the necessary inputs");
        return 1;
    }
    else
    {
        if (argc <= 4)
        {

            IP = (char *)malloc((strlen(argv[1]) + 1) * sizeof(char));
            strcpy(IP, argv[1]);
            TCP = (char *)malloc((strlen(argv[2]) + 1) * sizeof(char));
            strcpy(TCP, argv[2]);
            regIP = (char *)malloc((strlen("193.136.138.142") + 1) * sizeof(char));
            strcpy(regIP, "193.136.138.142");
            regUDP = (char *)malloc((strlen("59000") + 1) * sizeof(char));
            strcpy(regUDP, "59000");
        }
        else
        {

            IP = (char *)malloc((strlen(argv[1]) + 1) * sizeof(char));
            strcpy(IP, argv[1]);
            TCP = (char *)malloc((strlen(argv[2]) + 1) * sizeof(char));
            strcpy(TCP, argv[2]);
            regIP = (char *)malloc((strlen(argv[3]) + 1) * sizeof(char));
            strcpy(regIP, argv[3]);
            regUDP = (char *)malloc((strlen(argv[4]) + 1) * sizeof(char));
            strcpy(regUDP, argv[4]);
        }
    }

    printf("\nDebug: len: %d, IP: %s, TCP: %s, regIP: %s, regUDP: %s\n", argc, IP, TCP, regIP, regUDP);

    node_s *my_node = create_node(IP, TCP, regIP, regUDP);

    printf("\nDebug: created node with IP: %s, TCP: %s, regIP: %s, regUDP: %s\n", my_node->ip, my_node->port, my_node->regIP, my_node->regUDP);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                               ciclo do programa com select                                                             //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    fd_set rfds;
    int counter;
    bool running = true;

    while (running)
    {
        node_state(my_node);

        if (my_node->left)
        {
            my_node->exiting = true;
            continue;
        }

        int maxfd = calc_maxfd(my_node);              /* fd é o maior dos descritores a monitorizar, dado que o descritor do stdin é 0 (menor que fd) */
        FD_ZERO(&rfds);                               /* remover todos os descritores do conjunto */
        FD_SET(0, &rfds);                             /* adicionar o descritor 0 (stdin) ao conjunto */
        FD_SET(my_node->udp_socket, &rfds);           /* adicionar o descritor do socket udp ao conjunto */
        FD_SET(my_node->tcp_listening_socket, &rfds); /* adicionar o descritor do socket server tcp ao conjunto */
        FD_SET(my_node->tcp_connected_socket, &rfds); /* adicionar o descritor do socket tcp de conexão temporária ao conjunto */
        for (int i = 0; i < 100; i++)
        {
            if (my_node->is_a_connected_neighbor[i] && my_node->neighbors[i] != NULL)
            {
                FD_SET(my_node->neighbors[i]->tcp_socket, &rfds); /* adicionar o descritor do socket tcp de cada vizinho ao conjunto */
            }
        }

        counter = select(maxfd + 1, &rfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
        if (counter == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else if (counter > 0)
        {
            if (FD_ISSET(0, &rfds)) // Input no stdin
            {
                printf("\nDebug: input detected on stdin\n");
                char buffer[256];
                fgets(buffer, sizeof(buffer), stdin);
                printf("Input recebido: %s\n", buffer);
                process_stdin_message(buffer, my_node);
            }
            else if (FD_ISSET(my_node->udp_socket, &rfds)) // Input no socket UDP
            {
                process_udp_message(my_node);
            }
            else if (FD_ISSET(my_node->tcp_listening_socket, &rfds)) // Input no socket server TCP
            {
                process_tcp_server_message(my_node);
            }
            else if (my_node->tcp_connected_socket != -1 && FD_ISSET(my_node->tcp_connected_socket, &rfds)) // Input no socket TCP de conexão temporária
            {
                process_tcp_connection_message(my_node, -1);
            }
            else // Input no socket TCP de um neighbor
            {
                for (int i = 0; i < 100; i++)
                {
                    if (my_node->is_a_connected_neighbor[i] && my_node->neighbors[i] != NULL)
                    {
                        if (FD_ISSET(my_node->neighbors[i]->tcp_socket, &rfds))
                        {
                            process_tcp_connection_message(my_node, i);
                            break; // Processar apenas um vizinho por iteração do loop
                        }
                    }
                }
            }
        }
        if (my_node->exiting)
        {
            printf("Exiting program...\n");
            running = false;
        }
    }

    // Dar free de tudo o que foi alocado dinamicamente
    free(IP);
    free(TCP);
    free(regIP);
    free(regUDP);
    free_node(my_node);

    return 0;
}