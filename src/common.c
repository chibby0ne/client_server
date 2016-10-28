#include "common.h"
#include "client.h"
#include "server.h"


void print_ip(struct addrinfo *res)
{
    char ipstr[INET6_ADDRSTRLEN];
    if (res->ai_family ==  AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) res->ai_addr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
    } else {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) res->ai_addr;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipstr, sizeof(ipstr));
    }
    fprintf(stderr, "%s\n", ipstr);

}

void initialize_hints(struct addrinfo *hints, int flag)
{
    assert(flag == SERVER || flag == CLIENT);
    // initialize hints to 0
    memset(hints, 0, sizeof(hints));
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    if (flag == SERVER) {
        hints->ai_flags = AI_PASSIVE;
    }
}

void get_addrinfo_list(const char *hostname, const char *port_number, struct
        addrinfo *hints, struct addrinfo **res)
{
    assert(hostname != "");
    assert(strtol(PORT_NUMBER, NULL, 10) > 1024);
    printf("preparing addrinfo struct...\n");
    // get the addrinfo structures list with the hints criteria
    int status = getaddrinfo(hostname, port_number, hints, res);
    if (status != 0) {
        fprintf(stderr, "get_addrinfo_list: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    assert(status == 0);
}

int find_connectable_socket(struct addrinfo *res)
{
    assert(res != NULL);
    printf("creating socket...\n");
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
            return socketfd;
        } 
    }
    assert(socketfd == -1);
    fprintf(stderr, "Couldn't find a socket\n");
    exit(EXIT_FAILURE);
}

int find_socket(struct addrinfo *res)
{
    assert(res != NULL);
    printf("creating socket...\n");
    int socketfd;
    /**
    * iterate over the list of addrinfo structure until we find an addressinfo
    * that we can create a socket from and connect to its address.
    */
    for (struct addrinfo *ai = res;  ai != NULL; ai = ai->ai_next) {
        socketfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

        // if the socket wasn't created, try with another addrinfo structure
        if (socketfd == -1) {
            perror("find_socket-socket()");
            continue;
        } else {
            return socketfd;
        }
    }
    assert(socketfd == -1);
    fprintf(stderr, "Couldn't find a socket\n");
    exit(EXIT_FAILURE);
}

int connect_through_socket(int socketfd, struct addrinfo *res)
{
    assert(socketfd != -1);
    assert(res != NULL);
    int ret = connect(socketfd, res->ai_addr, res->ai_addrlen);
    /**
    * if the socket was successfully connected we stop looking through the
    * list
    */
    if (ret == 0) {
        return 0;
    } else {
        perror("connect_through_socket-connect()");
        print_ip(res);
        /** 
        * close the file descriptor (socket is a file descriptor), as we
        * couldn't connect through it
        */
        close(socketfd);
        return -1;
    }

}

void bind_socket(int socketfd, struct addrinfo *res)
{
    assert(socketfd != -1);
    assert(res != NULL);
    printf("binding socket to port %s\n", PORT_NUMBER);
    int ret = bind(socketfd, res->ai_addr, res->ai_addrlen);
    if (ret == -1) {
        perror("bind_socket-bind()");
        if (errno == EADDRINUSE) {
            int yes = 1;

            if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                        sizeof(int)) == -1) {
                perror("setsockopt");
                exit(1);
            }
            printf("freeing port %s...\n", PORT_NUMBER);
        } else {
            exit(EXIT_FAILURE);
        }
    }
}

void listen_socket(int socketfd, int backlog)
{
    assert(socketfd != -1);
    printf("listening to port: %s\n", PORT_NUMBER);
    int ret = listen(socketfd, backlog);
    if (ret == -1) {
        perror("listen_socket-listen()");
        exit(EXIT_FAILURE);
    }
}

int accept_connection(int socketfd, struct sockaddr_storage *addr)
{
    assert(socketfd != -1);
    printf("accepting connections...\n");
    socklen_t addr_size = sizeof(*addr);
    int new_socket = accept(socketfd, (struct sockaddr *)addr, &addr_size);
    if (new_socket == -1) {
        assert(errno == ENOTSOCK);
        perror("accept_connection-accept()");
        exit(EXIT_FAILURE);
    }
    assert(new_socket != -1);
    return new_socket;
}

int receive_message(void *object, int type)
{
    int status = 0;
    struct client_t *client;
    struct server_t *server;
    switch (type) {
        case CLIENT:
            client = (struct client_t *) object;
            status = recv(client->socket_connected, &(client->recv_buffer),
                    BUFFER_SIZE, 0);
            break;
        case SERVER:
            server = (struct server_t *) object;
            status = recv(server->socket_connected, &(server->recv_buffer),
                    BUFFER_SIZE, 0);
            break;
        default:
            fprintf(stderr, "This is an unsupported mode of operation\n");
            exit(EXIT_FAILURE);
    }
    if (status == -1) {
        perror("receive_message-recv()");
    }
    return status;
}

void send_message(void *object, int type)
{
    int status;
    struct client_t *client;
    struct server_t *server;
    switch (type) {
        case CLIENT:
            client = (struct client_t *) object;
            status = send(client->socket_connected, &(client->send_buffer),
                    BUFFER_SIZE, 0);
            break;
        case SERVER:
            server = (struct server_t *) object;
            status = send(server->socket_connected, &(server->send_buffer),
                    BUFFER_SIZE, 0);
            break;
        default:
            fprintf(stderr, "This is an unsupported mode of operation\n");
            exit(EXIT_FAILURE);
    }
    if (status == -1) {
        perror("send_message-send()");
    }
}


/**
 * Prints message to console, and prepends a "From server/client" depending on
 * which type of program it is running as i.e: if it is running as a client,
 * then the message must come from the server
 *
 * @param buffer char array where the message is stored
 * @param type CLIENT or SERVER int macros
 */
void show_message(char *buffer, int type)
{
    printf("%s %s", type == CLIENT ? "From server:" : "From client:", buffer);
}

/**
 * Reads strings from stdin and stores it in a buffer up to the EOF or the
 * newline character, if newline character is inputted it is also included in
 * the message stored in the buffer
 *
 * @param buffer char array whre the message is stored
 */
void read_send_messages(char *buffer)
{
    fgets(buffer, BUFFER_SIZE, stdin);
}
