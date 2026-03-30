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

    //printf("Processing 'NEIGHBOR' message...\n");

    if (sscanf(received_message, "%49s %49s", cmd, id) == 2)
    {
        //printf("Processing 'NEIGHBOR' message...1\n");
        if (node->receiving_neighbor)
        {
            //printf("Processing 'NEIGHBOR' message...2\n");
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
    char dest_str[50] = {0};
    char n_str[50] = {0};

    if (sscanf(received_message, "%49s %49s %49s", cmd, dest_str, n_str) == 3)
    {

        if (node->monitoring)
        {
            printf("\nReceived %s from neighbor %d\n", received_message, id);
        }
        int dest = atoi(dest_str);
        int n = atoi(n_str);

        if (dest == atoi(node->id))
            return;

        if (node->state[dest] == 1)
            return; // se estiver em estado de coordenação não aceitar rotas para esse destino para evitar loops infinitos

        if ((n + 1) < node->dist[dest] || node->dist[dest] == -1)
        {
            node->dist[dest] = n + 1;
            node->succ[dest] = id;
            node->state[dest] = 0;

            char route_message[128];
            sprintf(route_message, "ROUTE %d %d\n", dest, node->dist[dest]);

            for (int i = 0; i < 100; i++)
            {
                if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
                {

                    if (node->monitoring)
                    {
                        printf("\nSent %s to neighbor %d\n", route_message, i);
                    }
                    send_message_tcp(node->neighbors[i]->tcp_socket, route_message);
                }
            }
        }
    }
}

void chat(node_s *node, const char *received_message, int id)
{
    char cmd[50] = {0};
    char origin[50] = {0};
    char dest_str[50] = {0};
    char msg[128] = {0};

    if (sscanf(received_message, "%49s %49s %49s %127[^\n]", cmd, origin, dest_str, msg) == 4)
    {
        if (node->monitoring)
        {
            printf("\nReceived %s from neighbor %d\n", received_message, id);
        }

        int dest = atoi(dest_str);

        if (dest == atoi(node->id))
        {
            printf("Received message from %s: %s\n", origin, msg);
        }
        else
        {
            if (node->succ[dest] != -1 && node->state[dest] == 0)
            {
                char chat_message[256];
                sprintf(chat_message, "CHAT %s %02d %s\n", origin, dest, msg);
                send_message_tcp(node->neighbors[node->succ[dest]]->tcp_socket, chat_message);
                if (node->monitoring)
                {
                    printf("\nSent %s to neighbor %d\n", chat_message, node->succ[dest]);
                }
            }
            else
            {
                //printf("No route to destination %02d. Message dropped.\n", dest);
            }
        }
    }
}

void coordenation_after_loss(node_s *node, int lost_id)
{
    for (int dest = 0; dest < 100; dest++)
    {
        // Se o vizinho perdido era o sucessor para chegar a dest, passar para estado de coordenação relativamente a dest e informar os vizinhos
        if (node->succ[dest] == lost_id)
        {
            node->dist[dest] = -1;
            node->succ[dest] = -1;
            node->state[dest] = 1;
            node->succ_coord[dest] = -1;

            for (int i = 0; i < 100; i++)
            {
                if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
                {
                    node->coord[dest][i] = 1;

                    char coord_message[128];
                    sprintf(coord_message, "COORD %d\n", dest);
                    if (node->monitoring)
                    {
                        printf("\nSent %s to neighbor %d\n", coord_message, i);
                    }
                    send_message_tcp(node->neighbors[i]->tcp_socket, coord_message);
                }
            }
        }
    }
}

// Receção de uma mensagem tipo coordenação
// COORD dest<LF>
// Um nó informa um vizinho de que entrou no estado de coordenação relativamente
// ao destino dest.
// Suponha que o nó recebe do vizinho 𝑗 a mensagem (𝑐𝑜𝑜𝑟𝑑, 𝑡).
// 1. Se 𝑠𝑡𝑎𝑡𝑒[𝑡] = 1, então envia (uncoord, 𝑡) a 𝑗.
// 2. Se 𝑠𝑡𝑎𝑡𝑒[𝑡] = 0 e 𝑗 ≠ 𝑠𝑢𝑐𝑐[𝑡], então envia (𝑟𝑜𝑢𝑡𝑒, 𝑡, 𝑑𝑖𝑠𝑡) e (uncoord, 𝑡) a 𝑗.
// 3. Se 𝑠𝑡𝑎𝑡𝑒[𝑡] = 0 e 𝑗 = 𝑠𝑢𝑐𝑐[𝑡], então 𝑠𝑡𝑎𝑡𝑒[𝑡] : = 1; 𝑠𝑢𝑐𝑐_𝑐𝑜𝑜𝑟𝑑[𝑡] ∶=
// 𝑠𝑢𝑐𝑐[𝑡]; 𝑑𝑖𝑠𝑡[𝑡] ∶= ∞; 𝑠𝑢𝑐𝑐[𝑡] ≔ −1. Adicionalmente, para todo o vizinho 𝑘,
// envia (𝑐𝑜𝑜𝑟𝑑, 𝑡) a 𝑘 e 𝑐𝑜𝑜𝑟𝑑[𝑡, 𝑘] ≔ 1.

void coord(node_s *node, int id, char *received_message)
{
    char cmd[50] = {0};
    char dest[50] = {0};

    if (sscanf(received_message, "%49s %49s", cmd, dest) == 2)
    {
        if (node->monitoring)
        {
            printf("\nReceived %s from neighbor %d\n", received_message, id);
        }
        int d = atoi(dest);

        if (node->state[d] == 1)
        {
            char uncoord_message[128];
            sprintf(uncoord_message, "UNCOORD %d\n", d);
            if (node->monitoring)
            {
                printf("\nSent %s to neighbor %d\n", uncoord_message, id);
            }
            send_message_tcp(node->neighbors[id]->tcp_socket, uncoord_message);
        }
        else if (node->state[d] == 0 && id != node->succ[d])
        {
            char uncoord_message[128];
            sprintf(uncoord_message, "UNCOORD %d\n", d);
            if (node->monitoring)
            {
                printf("\nSent %s to neighbor %d\n", uncoord_message, id);
            }
            send_message_tcp(node->neighbors[id]->tcp_socket, uncoord_message);

            if (node->dist[d] != -1) // pode ser desnecessário verificar se é -1
            {
                char route_message[128];
                sprintf(route_message, "ROUTE %d %d\n", d, node->dist[d]);
                if (node->monitoring)
                {
                    printf("\nSent %s to neighbor %d\n", route_message, id);
                }
                send_message_tcp(node->neighbors[id]->tcp_socket, route_message);
            }
        }
        else if (node->state[d] == 0 && id == node->succ[d])
        {
            node->state[d] = 1;
            node->succ_coord[d] = id;
            node->dist[d] = -1;
            node->succ[d] = -1;

            for (int i = 0; i < 100; i++)
            {
                if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
                {
                    node->coord[d][i] = 1;

                    char coord_message[128];
                    sprintf(coord_message, "COORD %d\n", d);
                    send_message_tcp(node->neighbors[i]->tcp_socket, coord_message);
                    if (node->monitoring)
                    {
                        printf("\nSent %s to neighbor %d\n", coord_message, i);
                    }
                }
            }
        }
    }
}

// UNCOORD dest<LF>
// Um nó informa um vizinho que previamente lhe enviou uma mensagem de
// coordenação de que não depende desse vizinho para alcançar o destino dest.
// Receção de uma mensagem tipo expedição
// Suponha que o nó recebe do seu vizinho 𝑗 a mensagem (uncoord, 𝑡).
// 1. Se 𝑠𝑡𝑎𝑡𝑒[𝑡] = 1, então 𝑐𝑜𝑜𝑟𝑑[𝑡, 𝑗] ∶= 0.
// 2. Se 𝑐𝑜𝑜𝑟𝑑[𝑡, 𝑘] = 0 para todo o vizinho 𝑘, então 𝑠𝑡𝑎𝑡𝑒[𝑡] ∶= 0. Se 𝑑𝑖𝑠𝑡[𝑡] ≠ ∞,
// então envia (𝑟𝑜𝑢𝑡𝑒, 𝑡, 𝑑𝑖𝑠𝑡) a todos vizinhos. Se 𝑠𝑢𝑐𝑐_𝑐𝑜𝑜𝑟𝑑[𝑡] ≠ −1, então
// envia (uncoord, 𝑡) a 𝑠𝑢𝑐𝑐_𝑐𝑜𝑜𝑟𝑑[𝑡].

void uncoord(node_s *node, int id, char *received_message)
{
    char cmd[50] = {0};
    char dest_str[50] = {0};

    if (sscanf(received_message, "%49s %49s", cmd, dest_str) == 2)
    {
        if (node->monitoring)
        {
            printf("\nReceived %s from neighbor %d\n", received_message, id);
        }
        int dest = atoi(dest_str);

        if (node->state[dest] == 1)
        {
            node->coord[dest][id] = 0;

            // Verificar se todos os vizinhos ja nao estão em coordenação para dest, coord[dest][k] == 0 para todo k
            bool all_uncoord = true;
            for (int i = 0; i < 100; i++)
            {
                if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
                {
                    if (node->coord[dest][i] == 1)
                    {
                        all_uncoord = false;
                        break;
                    }
                }
            }

            if (all_uncoord)
            {
                node->state[dest] = 0;

                if (node->dist[dest] != -1)
                {
                    char route_message[128];
                    sprintf(route_message, "ROUTE %d %d\n", dest, node->dist[dest]);

                    for (int i = 0; i < 100; i++)
                    {
                        if (node->is_a_connected_neighbor[i] && node->neighbors[i] != NULL)
                        {
                            if (node->monitoring)
                            {
                                printf("\nSent %s to neighbor %d\n", route_message, i);
                            }
                            send_message_tcp(node->neighbors[i]->tcp_socket, route_message);
                        }
                    }
                }

                // Se tinha um sucessor de coordenação, enviar uncoord para esse sucessor de coordenação

                int succ_coord_id = node->succ_coord[dest];
                if (succ_coord_id != -1 && node->is_a_connected_neighbor[succ_coord_id])
                {
                    char uncoord_message[128];
                    sprintf(uncoord_message, "UNCOORD %d\n", dest);
                    if (node->monitoring)
                    {
                        printf("\nSent %s to neighbor %d\n", uncoord_message, succ_coord_id);
                    }
                    send_message_tcp(node->neighbors[succ_coord_id]->tcp_socket, uncoord_message);
                }
            }
        }
    }
}