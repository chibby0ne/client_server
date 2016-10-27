#ifndef GUARD_CLIENT_H
#define GUARD_CLIENT_H

#include "common.h"

struct client_t {
    int family;         // AF_INET or AF_INET6
    int socket_connected;   // socket connected to server
    char buffer[BUFFER_SIZE];   // buffer used for messages to receive/send
};

/* send message */
void send_message(struct client_t *client);

/* connect */
int connect_to_server(char *hostname, struct client_t *client);

/* disconnect */
void disconnect(struct client_t *client);

#endif /* ifndef GUARD_CLIENT_H */
