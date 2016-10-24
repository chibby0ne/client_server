#include "common.h"

void initialize_hints(struct addrinfo *hints, int flag)
{

    // initialize hints to 0
    memset(hints, 0, sizeof(hints));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    if (flag == SERVER) {
        hints->ai_flags = AI_PASSIVE;
    }
}

void get_addrinfo_list(const char *hostname, const char *port_number, struct
        addrinfo *hints, struct addrinfo **res)
{
    // get the addrinfo structures list with the hints criteria
    int status = getaddrinfo(hostname, port_number, hints, res);
    if (status != 0) {
        fprintf(stderr, "get_addrinfo_list: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
}

int find_connectable_socket(struct addrinfo *res)
{
    int socketfd;
    // iterate over the list of addrinfo structure until we find an addressinfo
    // that we can create a socket from and connect to its address.
    for (struct addrinfo *ai = res;  ai != NULL; ai = ai->ai_next) {
        socketfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

        // if the socket wasn't created, try with another addrinfo structure
        if (socketfd == -1) {
            perror("find_usable_socket-socket()");
            continue;
        }

        if (connect_through_socket(socketfd, ai) == 0) {
            break;
        } 

    }
    return socketfd;
}

int find_socket(struct addrinfo *res)
{
    int socketfd;
    // iterate over the list of addrinfo structure until we find an addressinfo
    // that we can create a socket from and connect to its address.
    for (struct addrinfo *ai = res;  ai != NULL; ai = ai->ai_next) {
        socketfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

        // if the socket wasn't created, try with another addrinfo structure
        if (socketfd == -1) {
            perror("find_usable_socket-socket()");
            continue;
        } else {
            break;
        }
    }
    return socketfd;

}

int connect_through_socket(int socketfd, struct addrinfo *res)
{
         int ret = connect(socketfd, res->ai_addr, res->ai_addrlen);
        // if the socket was successfully connected we stop looking through the
        // list
        if (ret == 0) {
            return 0;
        } else {
            perror("find_usable_socket-connect()");
        // close the file descriptor (socket is a file descriptor), as we
        // couldn't connect through it
            close(socketfd);
            return -1;
        }
 
}

void bind_socket(int socketfd, struct addrinfo *result)
{
    int ret = bind(socketfd, result->ai_addr, result->ai_addrlen);
    if (ret == -1) {
        perror("bind_socket-bind()");
        exit(EXIT_FAILURE);
    }
}

void listen_socket(int socketfd, int backlog)
{
    int ret = listen(socketfd, backlog);
    if (ret == -1) {
        perror("listen_socket-listen()");
        exit(EXIT_FAILURE);
    }
}

void accept_connection(int socketfd, struct sockaddr_storage *addr)
{
    socklen_t addr_size = sizeof (*addr);
    int new_socket = accept(socketfd, (struct sockaddr *) &addr, &addr_size); 
    if (new_socket == -1) {
        perror("accept_connection-accept()");
        exit(EXIT_FAILURE);
    }
}
