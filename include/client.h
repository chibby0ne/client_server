/**
 * Copyright (C) 2016 Antonio Gutierrez
 *
 * @brief Client structure and functions
 * @file client.h
 *
 */
#ifndef GUARD_CLIENT_H
#define GUARD_CLIENT_H

#include "common.h"

/**
 * @brief Client structure 
 *
 * Structure that represents the client. It contains the af_family, the
 * connected socket and the buffers for sending and receiving.
 */
struct client_t {
    int family;         /**< AF_INET or AF_INET6 */
    int socket_connected;   /**< socket connected to server */
    char recv_buffer[BUFFER_SIZE];   /**< buffer used for messages to receive */
    char send_buffer[BUFFER_SIZE];   /**< buffer used for messages to send */
};

/**
 * @brief Connects the given client to a server on the given hostname/IP
 * Connects the given client to a server on the given hostname/IP
 *
 * @param hostname hostname or ip address (can be either IPv6 or IPv4)
 * @param client client structure
 * @return a connected socket
 */
int connect_to_server(char *hostname, struct client_t *client);

/**
 * @brief Disconnects the given client 
 *
 * Disconnects the given client 
 *
 * @param client the given client with a connected socket
 */
void disconnect(struct client_t *client);

#endif /* ifndef GUARD_CLIENT_H */
