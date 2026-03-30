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
    bool loss_of_connection; // in the process of handling a loss of connection
    bool ready_to_exit; // ready to exit the program after finishing all processes
    bool monitoring; // in the process of monitoring the connections with neighbors
    bool direct_joined; // if the node joined directly to another node without going through the registry

    // neighbors
    int n_neighbors;              // number of neighbors
    bool *is_a_connected_neighbor; // array of booleans indicating if the node has a neighbor in each possible position (0-99)
    neighbor_s **neighbors;        // array of pointers to neighbor structures, indexed by neighbor id (0-99)

    // sockets
    int udp_socket;           // UDP socket to communicate with node servegvr
    int tcp_listening_socket; // TCP socket to listen for incoming connections from other nodes
    int tcp_connected_socket; // TCP socket for temporary befpre receiving the first message

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Forwarding protocol
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    int dist[100]; // distance to each neighbor (0-99)   (Unreachable = -1)
    int succ[100]; // forwarding successor to get to in distance dist[neighbor] for each neighbor (0-99)  (Unreachable = -1)
    int state[100]; // state relative to each neighbor (0-99)  (0 = expedition, 1 = coordenation)
    int succ_coord[100]; // id of the neighbor that started the coordination state, if its by fail of the connection becomes -1
    int coord[100][100]; // coord [t][j] - state relative to t, if j is on coordenation process 1 or not 0 


} node_s;


node_s *create_node(char *ip, char *port, char *regIP, char *regUDP);
void free_node(node_s *node);
neighbor_s* create_neighbor(const char* id, const char* ip, const char* port);
void free_neighbor(neighbor_s* neighbor);

#endif