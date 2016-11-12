/**
 * Copyright (C) 2016 Antonio Gutierrez
 *
 * @brief Server structure and functions
 * @file server.h
 */
#ifndef GUARD_SERVER
#define GUARD_SERVER

#include "common.h"

/**
 * Structure that represents the server. It contains the af_family, the
 * listening socket, the connected socket and the buffers for sending and
 * receiving.
 */
struct server_t {
    int family;         /**< AF_INET or AF_INET6 */
    int socket_listening;   /**< socket listening to port */
    int socket_connected;   /**< socket connected to client */
    char recv_buffer[BUFFER_SIZE];   /**< buffer used for messages to receive */
    char send_buffer[BUFFER_SIZE];   /**< buffer used for messages to send */
};

/**
 * Starts the server, listening for connections until one is
 * accepted. The server structure passed is initialized and contains all the
 * relevant information.
 *
 * @param server server_t instance that holds all the information to maintain
 * the communication and transmission of messages to/from the client
 */
void start_server(struct server_t *server);

#endif /* ifndef GUARD_SERVER */
