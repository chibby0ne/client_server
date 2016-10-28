#ifndef GUARD_SERVER
#define GUARD_SERVER

#include "common.h"

struct server_t {
    int family;         // AF_INET or AF_INET6
    int socket_listening;   // socket listening to port
    int socket_connected;   // socket connected to client
    char recv_buffer[BUFFER_SIZE];   // buffer used for messages to receive
    char send_buffer[BUFFER_SIZE];   // buffer used for messages to send
};

/* start server */
void start_server(struct server_t *server);

#endif /* ifndef GUARD_SERVER */
