#include "node.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void neighbor(node_s *node, const char *received_message)
{

    char cmd[50] = {0};
    char id[50] = {0};

    printf("Processing 'NEIGHBOR' message...\n");


    if (sscanf(received_message, "%49s %49s", cmd, id) == 2)
    {
        printf("Processing 'NEIGHBOR' message...1\n");
        if(node->receiving_neighbor) {
             printf("Processing 'NEIGHBOR' message...2\n");
            int index_id = atoi(id);
            node->n_neighbors++;
            node->is_a_connected_neighbor[index_id] = true;
            node->neighbors[index_id] = create_neighbor(id, NULL, NULL); // ip e port não são necessários para o processo de receber um neighbor
            node->neighbors[index_id]->tcp_socket = node->tcp_connected_socket; // associar o socket de conexão temporária ao vizinho
            node->tcp_connected_socket = -1; // resetar o socket de conexão temporária para evitar confusão com futuras conexões
            node->receiving_neighbor = false;
        }else {
            
        }
    }
}