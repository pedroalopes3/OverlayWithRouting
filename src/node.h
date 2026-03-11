#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// neighbor structure
typedef struct
{
    char id[3]; // neighbor id
    char *ip;   // neighbor IP address
    char *port; // neighbor port

    // socket to communicate with this neighbor
    int tcp_socket; // TCP socket to communicate with this neighbor

} neighbor_s;


// node structure
typedef struct
{
    char net[4];  // network id
    char id[3];   // node id
    char *ip;     // node IP address
    char *port;   // node port
    char *regIP;  // registry IP address
    char *regUDP; // registry UDP port
    char tid[5];  // transaction with node server id

    // ongoing processes && processes status
    bool joining;     // in the process of joining the network
    bool joined;      // successfully joined the network
    bool leaving;     // in the process of leaving the network
    bool left;        // successfully left the network
    bool exiting;     // in the process of exiting the program
    bool adding_edge; // in the process of adding an edge
    bool receiving_neighbor; // in the process of receiving a neighbor connection

    // neighbors
    int n_neighbors;              // number of neighbors
    bool *is_a_connected_neighbor; // array of booleans indicating if the node has a neighbor in each possible position (0-99)
    neighbor_s **neighbors;        // array of pointers to neighbor structures, indexed by neighbor id (0-99)

    // sockets
    int udp_socket;           // UDP socket to communicate with node servegvr
    int tcp_listening_socket; // TCP socket to listen for incoming connections from other nodes
    int tcp_connected_socket; // TCP socket for temporary befpre receiving the first message
} node_s;


node_s *create_node(char *ip, char *port, char *regIP, char *regUDP);
void free_node(node_s *node);
neighbor_s* create_neighbor(const char* id, const char* ip, const char* port);
void free_neighbor(neighbor_s* neighbor);

#endif