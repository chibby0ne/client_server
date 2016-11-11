#include "common.h"
#include "client.h"
#include "server.h"

int main(int argc, char *argv[])
{
    int mode = handle_input(argc, argv);
    struct client_t *client;
    struct server_t *server;
    if (mode == CLIENT) {
        client = (struct client_t *) malloc(sizeof(struct client_t));
        connect_to_server(argv[3], client);
    } else {
        server = (struct server_t *) malloc(sizeof(struct server_t));
        start_server(server);
    }
    printf("Made a connection/Started server\n");

    int status = 1;
    if (mode == CLIENT) {
        while (status > 0) {
            read_send_messages(client->send_buffer);
            send_message(client, CLIENT);
            status = receive_message(client, CLIENT);
            show_message(client->recv_buffer, CLIENT);
        }
    } else {
        while (status > 0) {
            status = receive_message(server, SERVER);
            read_send_messages(server->send_buffer);
            send_message(server, SERVER);
            show_message(server->recv_buffer, SERVER);
        }
    }
    return 0;
}
