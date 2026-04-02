# OverlayWithRouting (OWR)

A peer-to-peer overlay network application with a distance-vector routing protocol, written in C. Each node joins a named network, connects to neighbours over TCP, and can send chat messages that are forwarded hop-by-hop using the routing tables maintained by each node.

---

## Features

- **Overlay network** – nodes discover each other through a UDP-based node registry
- **Distance-vector routing** – every node maintains distance, successor, and coordination-state tables for all reachable nodes
- **TCP neighbour connections** – persistent TCP links between neighbours for routing and chat messages
- **Interactive CLI** – all commands are entered on standard input at runtime
- **I/O multiplexing** – a single `select()` loop handles stdin, UDP, and all TCP sockets concurrently

---

## Requirements

- GCC (with C99 / GNU99 support)
- GNU Make
- Linux / macOS (POSIX sockets)

---

## Building

```bash
cd src
make
```

This produces the `owr` binary. To remove build artefacts:

```bash
make clean
```

---

## Usage

```
./owr IP TCP [regIP regUDP]
```

| Argument | Description |
|----------|-------------|
| `IP`     | This node's IP address |
| `TCP`    | TCP port this node listens on |
| `regIP`  | *(optional)* Registry server IP (default: `193.136.138.142`) |
| `regUDP` | *(optional)* Registry server UDP port (default: `59000`) |

### Example

```bash
./owr 192.168.1.10 58000
```

---

## Interactive Commands

Once running, type commands at the prompt:

| Command | Description |
|---------|-------------|
| `join <net> <id>` | Join overlay network `<net>` with node ID `<id>` (via registry) |
| `djoin <net> <id>` | Direct join: join without using the registry |
| `show nodes [net]` | List nodes registered in network `<net>` |
| `show neighbors` | List the current node's direct neighbours |
| `show routing <dest>` | Show routing-table entry for destination node `<dest>` |
| `add edge <id>` | Add a TCP edge to node `<id>` (resolved via registry) |
| `dadd edge <id> <ip> <port>` | Direct add edge: connect directly using known address |
| `remove edge <id>` | Remove the TCP edge to neighbour `<id>` |
| `announce` | Broadcast a ROUTE message to all neighbours |
| `message <dest> <text>` | Send a chat message to node `<dest>` |
| `leave` | Unregister from the network and disconnect from all neighbours |

---

## Project Structure

```
src/
├── main.c          # Entry point; select() event loop
├── node.c / .h     # Node and neighbour data structures
├── general.c / .h  # High-level command handlers (join, leave, routing, …)
├── stdin_msg.c/.h  # Stdin message dispatcher
├── tcp.c / .h      # TCP socket helpers
├── tcp_msg.c / .h  # TCP message processor
├── udp.c / .h      # UDP socket helpers
├── udp_msg.c / .h  # UDP message processor (registry protocol)
└── makefile
doc/                # Project specification and reference materials
```

---

## Protocol Overview

### Registry (UDP)

The node registry is contacted via UDP to register, unregister, and look up nodes:

- `NODES <tid> 0 <net>` – request the list of nodes in a network
- `REG <tid> 3 <net> <id>` – unregister from a network
- `CONTACT <tid> 0 <net> <id>` – request the contact info for node `<id>`

### Neighbour connections (TCP)

When two nodes connect, they exchange `NEIGHBOR <id>` handshake messages. The routing protocol then propagates `ROUTE <id> <dist>` messages to maintain distance-vector tables across the overlay.

### Chat (TCP)

`CHAT <origin> <dest> <text>` messages are forwarded hop-by-hop according to the routing table until they reach `<dest>`.

---

## License

This project was developed as a university assignment. See the `doc/` folder for the full project specification.
