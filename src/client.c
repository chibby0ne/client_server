#include "client.h"

/**
 * Connects the given client to a server on the given hostname/IP
 *
 * @param a hostname or ip address (can be either IPv6 or IPv4)
 * @return a connected socket
 */
int connect_to_server(char *hostname, char *port, struct client_t *client)
{
    struct addrinfo hints;
    initialize_hints(&hints, CLIENT);

    // getaddrinfo
    struct addrinfo *result;
    printf("connecting to %s in port %s\n", hostname, port);
    get_addrinfo_list(hostname, port, &hints, &result);

    // socket
    client->socket_connected = find_connectable_socket(result);

    // free structrure returned
    freeaddrinfo(result);

    return client->socket_connected;
}


/**
 * Disconnects the given client 
 *
 * @param client the given client with a connected socket
 */
void disconnect(struct client_t *client)
{
    close(client->socket_connected);
}
