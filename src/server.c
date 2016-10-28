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
    server->socket_connected = 0;
    server->socket_connected = accept_connection(server->socket_listening,&addr);
    assert(server->socket_connected > -1);

}
