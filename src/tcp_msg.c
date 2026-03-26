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
        if (node->receiving_neighbor)
        {
            printf("Processing 'NEIGHBOR' message...2\n");
            int index_id = atoi(id);
            node->n_neighbors++;
            node->is_a_connected_neighbor[index_id] = true;
            node->neighbors[index_id] = create_neighbor(id, NULL, NULL);        // ip e port não são necessários para o processo de receber um neighbor
            node->neighbors[index_id]->tcp_socket = node->tcp_connected_socket; // associar o socket de conexão temporária ao vizinho
            node->tcp_connected_socket = -1;                                    // resetar o socket de conexão temporária para evitar confusão com futuras conexões
            node->receiving_neighbor = false;
        }
        else
        {
        }
    }
}

void route(node_s *node, const char *received_message, int id)
{

    char cmd[50] = {0};
    char dest[50] = {0};
    char n[50] = {0};

    printf("Processing received 'ROUTE' message...\n");

    // Implementa a lógica básica de vetor de distâncias: se receberes uma distância para um destino que é menor
    // do que a que tens guardada no teu dist[dest], atualizas o dist[dest] para dist_recebida + 1,
    // atualizas o succ[dest] para o vizinho que te enviou a mensagem, e reenvias um novo ROUTE dest (dist_recebida + 1) aos teus outros vizinhos.

    if (sscanf(received_message, "%49s %49s %49s", cmd, dest, n) == 3)
    {
        if ((atoi(n) + 1) < node->dist[atoi(dest)] || (node->dist[atoi(dest)] == -1))
        {
            if (atoi(dest) == atoi(node->id))
            {
                return;
            }
            else
            {
                node->dist[atoi(dest)] = atoi(n) + 1;
                node->succ[atoi(dest)] = id;
            }

            if (node->state[atoi(dest)] == 0)
            {
                char route_message[256] = {0};
                strcat(route_message, "ROUTE ");
                strcat(route_message, dest);
                strcat(route_message, " ");
                char dist_str[50];
                sprintf(dist_str, "%d", node->dist[atoi(dest)]);
                strcat(route_message, dist_str);
                strcat(route_message, "\n");

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
        }
    }
}

void chat(node_s *node, const char *received_message, int id)
{
    char cmd[50] = {0};
    char origin[50] = {0};
    char dest[50] = {0};
    char msg[200] = {0};

    printf("Processing received 'CHAT' message...\n");

    if (sscanf(received_message, "%49s %49s %49s %199[^\n]", cmd, origin, dest, msg) == 4)
    {
        if (atoi(dest) == atoi(node->id))
        {
            printf("Received message from %s: %s\n", origin, msg);
        }
        else
        {

            if (node->succ[atoi(dest)] != -1)
            {
                char chat_message[256] = {0};
                strcat(chat_message, "CHAT ");
                strcat(chat_message, origin);
                strcat(chat_message, " ");
                strcat(chat_message, dest);
                strcat(chat_message, " ");
                strcat(chat_message, msg);
                strcat(chat_message, "\n");

                send_message_tcp(node->neighbors[node->succ[atoi(dest)]]->tcp_socket, chat_message);
            }
            else
            {
                printf("No route to destination %s. Message dropped.\n", dest);
            }
        }
    }
}

void coordenation_after_loss(node_s *node, int id)
{
    // descobrir os destinos para os quais o vizinho era o sucessor e colocar como não alcançáveis e guardar numa lista esses destinos
    for (int i = 0; i < 100; i++)
    {
        if (node->succ[i] == id)
        {
            node->dist[i] = -1;
            node->succ[i] = -1;
            node->state[i] = 1;
            node->succ_coord[i] = -1; // como se deu por falha e -1
        }
    }
    int neighbors_seen = 0;
    for (int i = 0; i < 100 && neighbors_seen <= node->n_neighbors; i++)
    {
        if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
        {
            neighbors_seen++;
            coord[id][i] = 1;
            char coord_message[256] = {0};
            strcat(coord_message, "COORD ");
            char id_str[50];
            sprintf(id_str, "%d", id);
            strcat(coord_message, id_str);
            strcat(coord_message, "\n");
            send_message_tcp(node->neighbors[i]->tcp_socket, coord_message);
        }
    }
}

void coord(node_s *node, int id, char *received_message)
{

}