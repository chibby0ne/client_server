#include "common.h"
#include "client.h"
#include "server.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Initializes the hints structure passed as parameters. according to the flag
 * parameters passed. The family choosen is unspecified i.e: could be either
 * IPv4 or IPv6, and the socket type choosen is a SOCK_STREAM.
 *
 * @param hints structure when the hints are hold
 * @param flag used to initialize the flags, for our purposes either SERVER or
 * CLIENT
 */
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

/**
 * Invokes the getaddrinfo function to get the addressinfo structure res
 * obtained for the given hostname, the given port_number and the hints
 * structure passed.
 * Function fails and the program exits when there's an error with the
 * getaddrinfo function
 *
 * @param hostname hostname or IP address
 * @param port_number port number to listen/connect to
 * @param hints addrinfo structure that holds the hints for the type of
 * addrinfo structure we want
 * @param res resulting addrinfo structure using the parameters
 */
void get_addrinfo_list(const char *hostname, const char *port_number, struct
        addrinfo *hints, struct addrinfo **res)
{
    assert(hostname != "");
    printf("preparing addrinfo struct...\n");
    // get the addrinfo structures list with the hints criteria
    int status = getaddrinfo(hostname, port_number, hints, res);
    if (status != 0) {
        fprintf(stderr, "get_addrinfo_list: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    assert(status == 0);
}

/**
 *
 * Finds and returns the first connectable socket using the addrinfo structure
 * provided.
 * It iterates over the available addrinfo structures pointed by res beginning
 * from res.
 * Function fails and exits the program if it could not find a connectable
 *
 * @param res addrinfo structure that includes the family, socket type and
 * protocol to be used for the socket
 * @return the connectable socket
 */
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

/**
 * Finds and returns the first socket found from the addrinfo structure res
 * NOTE: This socket may or may not be connectable. For a connectable socket
 * @see find_connectable_socket
 *
 * @param res structure holding the chain of addrinfo strucutres containing the
 * parameters necessary for the creation of a socket
 * @return the socket
 */
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

/** 
 * Tries to connect the given socketfd using the information contained in res
 * 
 * @return 0 on success, -1 otherwise
 */
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
        /** 
        * close the file descriptor (socket is a file descriptor), as we
        * couldn't connect through it
        */
        close(socketfd);
        return -1;
    }

}

/**
 * Helper function that prints the ip address from a struct addrinfo to stderr
 *
 * @param res pointer to structure holding the ip address
 */
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

/**
 * Binds the given socket to the port PORT_NUMBER, that was specifed when
 * obtaining the res structure.
 * This function is mostly used when want to create a server, as it is necessary
 * to bind the socket before starting to listen to incomming connections.
 * Fails and exits the program if an error occure while calling the bind
 * function on the socket
 *
 * @param socketfd socket used for conections
 * @param res structure that holds the parameters required to bind socketfd
 */
void bind_socket(int socketfd, char *port, struct addrinfo *res)
{
    assert(socketfd != -1);
    assert(res != NULL);
    printf("binding socket to port %s\n", port);
    int ret = bind(socketfd, res->ai_addr, res->ai_addrlen);
    if (ret == -1) {
        perror("bind_socket-bind()");
        // if the port is not in use for we get from the kernel that it is in
        // use, it is because it takes a while for the kernel to release the
        // port again, but this forces it to release it
        if (errno == EADDRINUSE) {
            int yes = 1;
            if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                        sizeof(int)) == -1) {
                perror("setsockopt");
                exit(1);
            }
            printf("freeing port %s...\n", port);
        } else {
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Listens to incomming using the socket socketfd, and holds a maximum number of
 * connections in queue of backlog
 * Fails and exits the program if there's an error while calling the listen
 * function on the socket
 *
 * @param socket used for incomming connections
 * @param backlog max number of incomming connections to be queued.
 */
void listen_socket(int socketfd, char *port, int backlog)
{
    assert(socketfd != -1);
    printf("listening to port: %s\n", port);
    int ret = listen(socketfd, backlog);
    if (ret == -1) {
        perror("listen_socket-listen()");
        exit(EXIT_FAILURE);
    }
}

/**
 * Wrapper for the accept function, that accepts an incoming connections from
 * the queue of incomming connections to socketfd, and stores the address of the
 * client into addr structure. It creates a socket with the accepted connnection
 * and the original socketfd remains open listening for more connections.
 *
 * @param socketfd listening socket that has a connection pending from a client
 * @param addr sockaddr_storage structure used to hold the address of the client 
 * @return the new socket that is connected to the remote socket 
 */
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

/**
 * Handles the reception of messages from a either a client or a server, using a
 * connected socket contained inside object, that is of the given type.
 * In case there's an error, the function prints out an error message to
 * stderr describing the problem.
 *
 * @param object client or server containing the connected socket
 * @param type indicates whether the object is a CLIENT or SERVER type
 * @return error status of the recv function call
 */
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

/**
 * Handles the sending of messages from either a client or a server, using a
 * connected socket contained insde boject, that is of the given type.
 * In case there's an error, the function prints out an error message to
 * stderr describing the problem.
 *
 * @param object client or server containing the connected socket
 * @param type indicates whether the object is a CLIENT or SERVER type
 * @return error status of the recv function call
 */
int send_message(void *object, int type)
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
    return status;
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
void read_stdin_to_buffer(char *buffer)
{
    fgets(buffer, BUFFER_SIZE, stdin);
}

/**
 * Read strings from stdint and sends them throught the connected socket of the
 * given client. This is the function handled by the recv_thread.
 *
 * @param client a structure containing the server and the status used to break
 * from the main loop
 */
void *read_received_message_client(void *client_param)
{
    struct client_recv_status_t *client_recv_status = (struct client_recv_status_t *)
        client_param;
    struct client_t *client = client_recv_status->client;
    int *status = client_recv_status->recv_status;
    do {
        *status = receive_message(client, CLIENT);
        show_message(client->recv_buffer, CLIENT);
    } while (*status > 0);
    return NULL;
}

/**
 * Read strings from stdint and sends them throught the connected socket of the
 * given server. This is the function handled by the recv_thread.
 *
 * @param server a structure containing the server and the status used to break
 * from the main loop
 */
void *read_received_message_server(void *server_param)
{
    struct server_recv_status_t *server_recv_status = (struct server_recv_status_t *)
        server_param;
    struct server_t *server = server_recv_status->server;
    int *status = server_recv_status->recv_status;
    do {
        *status = receive_message(server, SERVER);
        show_message(server->recv_buffer, SERVER);
    } while (*status > 0);
    return NULL;
}

/** 
 * Helper function used to create and return a string, which is a copy of the
 * string s but in which all alphabetical characters are lower case
 *
 * @param s string with characters in lower and/or upper case
 * @return string with characters in lower case
 */
char *convert_to_lowercase(char *s)
{
    size_t len = sizeof(s);
    char *new_s = (char *) malloc(len);
    for (u_int32_t i = 0; i < len; ++i) {
        new_s[i] = tolower(s[i]);
    }
    return new_s;
}

/**
 * Helper function used for showing fatal error messages on stderr
 */
void print_error_exit()
{
    fprintf(stderr, 
            "Usage: client_server MODE [IP]\nMODE: \"client\" or " 
            "\"server\"\nIP: only used for client mode (could be IP or "
            "hostname)\n");
    exit(EXIT_FAILURE);

}

/**
 * Checks if the string corresponds to a valid ip address
 *
 * @param s contains the ip address
 * @return 1 if s is valid, 0 otherwise
 */
int is_valid_ip(char *s)
{
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    int ret = inet_pton(AF_INET, s, &(sa.sin_addr)) ||
            inet_pton(AF_INET6, s, &(sa6.sin6_addr));
    return ret == 1;
}

/**
 * Checks if the s is a valid hostname
 *
 * @param s contains the hostname of the server to connect to
 * @return 1 if s is a valid hostname, 0 otherwise
 */
int is_valid_hostname(char *s)
{
    size_t len = sizeof(s);
    if (len > HOSTNAME_MAX_LENGTH) {
        return 0;
    }
    u_int32_t label_size = 0;
    for (u_int32_t i = 0; i < len; ++i) {
        if (s[i] == '.') {
            if (s[i - 1] == '.') {
                return 0;
            }
            label_size = 0;
        } else if (is_label_longer_than_allowed(label_size) ||
                !is_valid_hostname_char(s[i])) {
            return 0;
        }
        label_size++;
    }
    return 1;
}

/**
 * Checks if label is longer than maximum allowed
 *
 * @param number of chars between the last dot (or beginning) and now
 * @return 1 if it longer than allowed, 0 otherwise
 */
int is_label_longer_than_allowed(u_int32_t size_between_dots)
{
    return size_between_dots > HOSTNAME_LABEL_MAX_LENGTH;
}

/**
 * Checks if c is a valid hostname character
 *
 * @param c character to check
 * @return 1 if c is a valid hostname char, 0 otherwise
 */
int is_valid_hostname_char(char c)
{
    return (isalpha(c) || c == '-' || c == '_' || c == '.');
}


/**
 * Prints the command line parameters to stdout, including the executable name
 *
 * @param argc number of command line arguments (including the executable)
 * @param argv array of strings representing command line arguments
 */
void print_cla(int argc, char *argv[])
{
    for (u_int32_t i = 0; i < argc; ++i) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}

/**
 * Checks the command line arguments, validates them, and returns the mode
 * chosen for the current program.
 *
 * @param argc number of parameters (including executable name)
 * @param argv array of command line parameters
 * @return the mode to use the program (CLIENT or SERVER)
 */
int handle_input(int argc, char *argv[])
{
    int mode;
    if (argc < 2 || argc > 4) {
        print_cla(argc, argv);
        print_error_exit();
    } else {
        if (strcmp("client", convert_to_lowercase(argv[1])) == 0) {
            mode = CLIENT;
            if (argc == 3) {
                if (!is_valid_ip(argv[2]) && !is_valid_hostname(argv[2])) {
                    fprintf(stderr, "Not valid IP or hostname: %s\n", argv[2]);
                    print_error_exit();
                }
            } else if (argc == 4) {
                if (!is_valid_port(argv[3])) {
                    fprintf(stderr, "Not valid port number: %s. Enter port "
                            "number in range: %d - %d\n", argv[3],
                            MIN_PORT_NUMBER, MAX_PORT_NUMBER);
                    print_error_exit();
                }
            }
        } else if (strcmp("server", convert_to_lowercase(argv[1])) == 0) {
            mode = SERVER;
            if (!is_valid_port(argv[2])) {
                fprintf(stderr, "Not valid port number: %s\n", argv[2]);
                print_error_exit();
            }
        } else {
            fprintf(stderr, "Not a valid mode: %s\n", argv[1]);
            print_error_exit();
        }
    }
    return mode;
}

/**
 * Utility function to aid in debugging of the addrinfo structure returned by
 * the call to getadrinfo. It prints all the IP address in the addrinfo chain.
 *
 * @param  addrinfo structure returned by getaddrinfo()
 */
void iterate_over_results(struct addrinfo *results)
{
    u_int32_t i = 0;
    for (struct addrinfo *cur = results; cur != NULL ; cur = cur->ai_next, i++)
    {
        printf("Number %d:\n", i);
        print_ip(cur);
    }
}

/**
 * Checks if s is a valid port number (between 1024 and 49151)
 *
 * @param s port number
 * @return 1 if it is a valid port, 0 otherwise
 */
int is_valid_port(char *s)
{
    int val = atoi(s);
    if (val >= MIN_PORT_NUMBER && val <= MAX_PORT_NUMBER) {
        return 1;
    }
    return 0;
}
