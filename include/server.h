#ifndef GUARD_SERVER
#define GUARD_SERVER

#include "common.h"

struct server_t {
    int socket_listening;
    int socket_connected;
    char buffer[BUFFER_SIZE];
};

/* start server */
void start_server(struct server_t *server);

/* received message */
void receive_message(struct server_t *server);

/* show message */
void show_message(struct server_t *server);


#endif /* ifndef GUARD_SERVER */
