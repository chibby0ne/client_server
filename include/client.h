#ifndef GUARD_CLIENT_H
#define GUARD_CLIENT_H

#include "common.h"

struct client_t {
    int socket_connected;
    char buffer[BUFFER_SIZE];
};

/* send message */
void send_message(struct client_t *client);

/* connect */
int connect_to_server(char *hostname, struct client_t *client);

/* disconnect */
void disconnect(struct client_t *client);

#endif /* ifndef GUARD_CLIENT_H */
