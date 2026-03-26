#include "node.h"
#include "udp.h"
#include "tcp.h"
#include "udp_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void nodes(char *received_message, node_s *node)
{
    if (node->joining)
    {

        bool is_unique = true;

        char *token = strtok(received_message, " \n");

        if (token != NULL && strcmp(token, "NODES") == 0)
        {
            char *tid = strtok(NULL, " \n");
            char *op = strtok(NULL, " \n");
            char *net = strtok(NULL, " \n");

            while ((token = strtok(NULL, " \n")) != NULL)
            {

                if (strcmp(token, node->id) == 0)
                {
                    is_unique = false;
                    break; //
                }
            }
            if (is_unique)
            {
                printf("Success: My ID (%s) is unique in this network!\n", node->id);
                // node->joined = true;
                if (node->joining)
                {
                    // continuar o join process e enviar o reg
                    char reg_message[256] = {0};
                    strcat(reg_message, "REG ");
                    strcat(reg_message, node->tid);
                    strcat(reg_message, " 0 ");
                    strcat(reg_message, node->net);
                    strcat(reg_message, "  ");
                    strcat(reg_message, node->id);
                    strcat(reg_message, "  ");
                    strcat(reg_message, node->ip);
                    strcat(reg_message, "  ");
                    strcat(reg_message, node->port);
                    strcat(reg_message, "\n");
                    send_udp_message(node->udp_socket, node->regIP, node->regUDP, reg_message);
                }
            }
            else
            {
                printf("Error: My ID (%s) is already taken!\n", node->id);
                node->joining = false;
                node->net[0] = '\0';
                node->id[0] = '\0';
                // Handle collision (e.g., exit or ask the user for a new ID)
            }
        }
    }
    else
    {
        printf("\n%s\n", received_message);
    }
}

void reg(node_s *node, const char *received_message)
{

    char cmd[50] = {0};
    char tid[50] = {0};
    char op[50] = {0};
    char net[50] = {0};
    char id[50] = {0};

    if (sscanf(received_message, "%49s %49s %49s %49s %49s", cmd, tid, op, net, id) >= 3)
    {
        if ((strcmp(op, "1") == 0) && (node->joining))
        {
            printf("Registration successful for node ID: %s\n", id);
            node->joined = true;
            node->joining = false;
        }
        else if ((strcmp(op, "2") == 0) && (node->joining))
        {
            printf("Registration failed for node ID %s the network %s because its full\n", id, net);
            node->joining = false;
        }
        else if ((strcmp(op, "4") == 0) && (node->leaving))
        {
            printf("Unregistration successfully node ID %s from network %s\n", id, net);
            node->leaving = false;
            node->left = true;
            node->joined = false;
            node->net[0] = '\0';
            node->id[0] = '\0';
        }
        else if ((strcmp(op, "4") != 0) && (strcmp(op, "1") != 0) && (strcmp(op, "2") != 0))
        {
            printf("Received unknown REG message: %s\n", received_message);
            if (node->joining)
            {
                node->joining = false;
                node->net[0] = '\0';
                node->id[0] = '\0';
            }
            else if (node->leaving)
            {
                node->leaving = false;
            }
        }
    }
}

void contact(node_s *node, const char *received_message)
{

    char cmd[50] = {0};
    char tid[50] = {0};
    char op[50] = {0};
    char net[50] = {0};
    char id[50] = {0};
    char ip[50] = {0};
    char port[50] = {0};

    if (sscanf(received_message, "%49s %49s %49s %49s %49s %49s %49s", cmd, tid, op, net, id, ip, port) >= 3)
    {
        if ((strcmp(op, "1") == 0) && (node->adding_edge))
        {
            printf("The contact of node %s is ip:%s port:%s\n", id, ip, port);
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
        else if ((strcmp(op, "2") == 0) && (node->adding_edge))
        {

            printf("Node %s is not registered\n", id);
            node->adding_edge = false;
        }
        else
        {
            printf("Received unknown CONTACT message: %s\n", received_message);
        }
    }
}