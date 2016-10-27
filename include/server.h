#ifndef GUARD_SERVER
#define GUARD_SERVER

#include "common.h"

struct server_t {
    int family;         // AF_INET or AF_INET6
    int socket_listening;   // socket listening to port
    int socket_connected;   // socket connected to client
    char buffer[BUFFER_SIZE];   // buffer used for messages to receive/send
};

/* start server */
void start_server(struct server_t *server);

/* received message */
void receive_message(struct server_t *server);

/* show message */
void show_message(struct server_t *server);


#endif /* ifndef GUARD_SERVER */
