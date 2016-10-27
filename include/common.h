#ifndef GUARD_COMMON_H
#define GUARD_COMMON_H

#define PORT_NUMBER "10000"

#define CLIENT 0
#define SERVER 1

#define BACKLOG_CONNECTIONS 20

#define BUFFER_SIZE 200

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

void initialize_hints(struct addrinfo *hints, int flags);
void get_addrinfo_list(const char *hostname, const char *port_number, struct
        addrinfo *hints, struct addrinfo **res);

void bind_socket(int socketfd, struct addrinfo *result);
int find_connectable_socket(struct addrinfo *addrinfo);
int find_socket(struct addrinfo *res);
int connect_through_socket(int socketfd, struct addrinfo *res);
void listen_socket(int socketfd, int backlog);
void accept_connection(int socketfd, struct sockaddr_storage *addr);

#endif /* ifndef GUARD_COMMON_H */
