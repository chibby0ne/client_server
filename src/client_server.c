#include "common.h"
#include "client.h"
#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char *argv[])
{
    int mode = handle_input(argc, argv);
    struct client_t *client;
    struct server_t *server;
    char *port;
    if (mode == CLIENT) {
        port = argv[3] == NULL ? DEFAULT_PORT_NUMBER : argv[3];
        client = (struct client_t *) malloc(sizeof(struct client_t));
            connect_to_server(argv[2], port, client);
    } else {
        port = argv[2] == NULL ? DEFAULT_PORT_NUMBER : argv[2];
        server = (struct server_t *) malloc(sizeof(struct server_t));
        start_server(port, server);
    }
    printf("Made a connection/Started server\n");

    int send_status;
    // initialize recv_status, because we don't want to leave the loop if we are
    // only stuff
    int recv_status = 1;
    pthread_t recv_thread;
    clear_screen();
    if (mode == CLIENT) {
        // create a structure that holds the client and a pointer to recv_status
        struct client_recv_status_t client_recv_status;
        client_recv_status.client = client;
        client_recv_status.recv_status = &recv_status;

        // run the reading of incomming messages on a separate thread
        if (pthread_create(&recv_thread, NULL, read_received_message_client,
                    (void *) &client_recv_status) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        // run the sending of outgoing messages in the main thread
        do {
            read_stdin_to_buffer(client->send_buffer);
            send_status = send_message(client, CLIENT);
        } while (send_status > 0 && recv_status > 0);
    } else {
        // create a structure that holds the server and a pointer to recv_status
        struct server_recv_status_t server_recv_status;
        server_recv_status.server = server;
        server_recv_status.recv_status = &recv_status;

        // run the reading of incomming messages on a separate thread
        if (pthread_create(&recv_thread, NULL, read_received_message_server,
                    (void *) &server_recv_status) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        // run the sending of outgoing messages in the main thread
        do {
            read_stdin_to_buffer(server->send_buffer);
            send_status = send_message(server, SERVER);
        } while (send_status > 0 && recv_status > 0);
    }
    pthread_join(recv_thread, NULL);
    return 0;
}
