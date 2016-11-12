#include "server.h"

/**
 * Starts the server, listening for connections until one is
 * accepted. The server structure passed is initialized and contains all the
 * relevant information.
 *
 * @param server server_t instance that holds all the information to maintain
 * the communication and transmission of messages to/from the client
 */
void start_server(struct server_t *server)
{
    struct addrinfo hints;
    initialize_hints(&hints, SERVER);

    // getaddrinfo
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
    server->socket_connected = accept_connection(server->socket_listening,&addr);
    assert(server->socket_connected > -1);
}
