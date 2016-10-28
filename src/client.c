#include "client.h"

/**
 * connects to client
 * @parameter a hostname or ip address
 * @return a connected socket
 */
int connect_to_server(char *hostname, struct client_t *client)
{
    struct addrinfo hints;
    initialize_hints(&hints, CLIENT);

    // getadrinfo
    struct addrinfo *result;
    get_addrinfo_list(hostname, PORT_NUMBER, &hints, &result);

    // socket
    client->socket_connected = find_connectable_socket(result);
    
    // free structrure returned
    freeaddrinfo(result);

}


void disconnect(struct client_t *client)
{
    close(client->socket_connected);
}
