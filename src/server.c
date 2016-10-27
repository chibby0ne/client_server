#include "server.h"

void start_server(struct server_t *server)
{
    struct addrinfo hints;
    initialize_hints(&hints, SERVER);

    // geatddrinfo
    struct addrinfo *result;
    get_addrinfo_list(NULL, PORT_NUMBER, &hints, &result);

    // socket
    server->socket_listening = find_socket(result);

    // bind
    bind_socket(server->socket_listening, result);

    // free structure returned
    freeaddrinfo(result);

    // listen
    listen_socket(server->socket_listening, BACKLOG_CONNECTIONS);

    // accept
    struct sockaddr_storage addr;
    accept_connection(server->socket_listening, &addr);

}


void receive_message(struct server_t *server)
{
    if (recv(server->socket_connected, (void *) &(server->buffer), BUFFER_SIZE, 0) == -1) {
        perror("receive_message-recv()");
    }
}

void show_message(struct server_t *server)
{
    printf("%s\n", server->buffer);
}
