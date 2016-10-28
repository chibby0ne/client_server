#include "common.h"
#include "client.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <arpa/inet.h>

char *tolowercase(char *s)
{
    size_t len = sizeof(s);
    char *new_s = (char *) malloc(len);
    for (u_int32_t i = 0; i < len; ++i) {
        new_s[i] = tolower(s[i]);
    }
    return new_s;
}

void print_error_exit()
{
    fprintf(stderr, 
            "Usage: client_server MODE [IP]\n MODE: \"client\" or " 
            "\"server\"\n IP: only used for client mode (could be IP or "
            "hostname)\n");
    exit(EXIT_FAILURE);

}

int is_valid_ip(char *s)
{
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    int ret = inet_pton(AF_INET, s, &(sa.sin_addr)) ||
            inet_pton(AF_INET6, s, &(sa6.sin6_addr));
    return ret == 1;
}

int has_alpha_only(char *s)
{
    size_t len = sizeof(s);
    for (u_int32_t i = 0; i < len; ++i) {
        if (!isalpha(s[i])) {
            return 0;
        }
    }
    return 1;
}

void print_cla(int argc, char *argv[])
{
    for (u_int32_t i = 0; i < argc; ++i) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}


int handle_input(int argc, char *argv[])
{
    int mode;
    if (argc < 2 || argc > 3) {
        print_cla(argc, argv);
        print_error_exit();
    } else {
        if (strcmp("client", tolowercase(argv[1])) == 0) {
            mode = CLIENT;
        } else if (strcmp("server", tolowercase(argv[1])) == 0) {
            mode = SERVER;
        } else {
            print_cla(argc, argv);
            print_error_exit();
        }
        if (argc == 3) {
            if (!is_valid_ip(argv[2]) && !has_alpha_only(argv[2])) {
                print_cla(argc, argv);
                print_error_exit();
            }
        }
    }
    return mode;

}

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
