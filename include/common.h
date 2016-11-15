/**
 * Copyright (C) 2016 Antonio Gutierrez
 *
 * @brief common and utility structures and functions
 * @file common.h
 */
#ifndef GUARD_COMMON_H
#define GUARD_COMMON_H

/** Port number used for the program */
#define DEFAULT_PORT_NUMBER "10000"

/** client */
#define CLIENT 0

/** server */
#define SERVER 1

/** number of maximum incomming connections in backlog */
#define BACKLOG_CONNECTIONS 20

/** max size of sending and receive buffers */
#define BUFFER_SIZE 200

/** max length of hostname */
#define HOSTNAME_MAX_LENGTH 253

/** max length of labels in hostname */
#define HOSTNAME_LABEL_MAX_LENGTH 63

/** minimum port number */
#define MIN_PORT_NUMBER 1024

/** maximum port number */
#define MAX_PORT_NUMBER 49151

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
#include <ctype.h>

/**
 * Utility structure used for it as argument to the thread handling the
 * reception of messages. It contains the client structure and the receive
 * status
 */
struct client_recv_status_t {
    struct client_t *client;    /**< client structure */
    int *recv_status;           /**< reference to the recv status */
};

/**
 * Utility structure used for it as argument to the thread handling the
 * reception of messages. It contains the server structure and the receive
 * status
 */
struct server_recv_status_t {
    struct server_t *server;    /**< server structure */
    int *recv_status;           /**< reference to the recv status */
};

/**
 * Initializes the hints structure passed as parameters. according to the flag
 * parameters passed. The family choosen is unspecified i.e: could be either
 * IPv4 or IPv6, and the socket type choosen is a SOCK_STREAM.
 *
 * @param hints structure when the hints are hold
 * @param flags used to initialize the flags, for our purposes either SERVER or
 * CLIENT
 */
void initialize_hints(struct addrinfo *hints, int flags);

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
        addrinfo *hints, struct addrinfo **res);

/**
 *
 * Finds and returns the first connectable socket using the addrinfo structure
 * provided.
 * It iterates over the available addrinfo structures pointed by res beginning
 * from res.
 * Function fails and exits the program if it could not find a connectable
 *
 * @param addrinfo addrinfo structure that includes the family, socket type and
 * protocol to be used for the socket
 * @return the connectable socket
 */
int find_connectable_socket(struct addrinfo *addrinfo);

/**
 * Finds and returns the first socket found from the addrinfo structure res
 * NOTE: This socket may or may not be connectable. For a connectable socket
 * @see find_connectable_socket
 *
 * @param res structure holding the chain of addrinfo strucutres containing the
 * parameters necessary for the creation of a socket
 * @return the socket
 */
int find_socket(struct addrinfo *res);

/** 
 * Tries to connect the given socketfd using the information contained in res
 *
 * @param socketfd open socket
 * @param res adrinfo structure that holds the connection information
 * @return 0 on success, -1 otherwise
 */
int connect_through_socket(int socketfd, struct addrinfo *res);

/**
 * Helper function that prints the ip address from a struct addrinfo to stderr
 *
 * @param res pointer to structure holding the ip address
 */
void print_ip(struct addrinfo *res);

/**
 * Binds the given socket to the port PORT_NUMBER, that was specifed when
 * obtaining the res structure.
 * This function is mostly used when want to create a server, as it is necessary
 * to bind the socket before starting to listen to incomming connections.
 * Fails and exits the program if an error occure while calling the bind
 * function on the socket
 *
 * @param socketfd socket used for conections
 * @param result structure that holds the parameters required to bind socketfd
 */
void bind_socket(int socketfd, char *port, struct addrinfo *result);

/**
 * Listens to incomming using the socket socketfd, and holds a maximum number of
 * connections in queue of backlog
 * Fails and exits the program if there's an error while calling the listen
 * function on the socket
 *
 * @param socketfd socket used for incomming connections
 * @param backlog max number of incomming connections to be queued.
 */
void listen_socket(int socketfd, char *port, int backlog);

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
int accept_connection(int socketfd, struct sockaddr_storage *addr);

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
int receive_message(void *object, int type);

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
int send_message(void *object, int type);

/**
 * Prints message to console, and prepends a "From server/client" depending on
 * which type of program it is running as i.e: if it is running as a client,
 * then the message must come from the server
 *
 * @param buffer char array where the message is stored
 * @param type CLIENT or SERVER int macros
 */
void show_message(char *buffer, int type);

/**
 * Reads strings from stdin and stores it in a buffer up to the EOF or the
 * newline character, if newline character is inputted it is also included in
 * the message stored in the buffer
 *
 * @param buffer char array whre the message is stored
 */
void read_stdin_to_buffer(char *buffer);

/**
 * Read strings from stdint and sends them throught the connected socket of the
 * given client. This is the function handled by the recv_thread.
 *
 * @param client_param a structure containing the server and the status used to break
 * from the main loop
 */
void *read_received_message_client(void *client_param);

/**
 * Read strings from stdint and sends them throught the connected socket of the
 * given server. This is the function handled by the recv_thread.
 *
 * @param server_param a structure containing the server and the status used to break
 * from the main loop
 */
void *read_received_message_server(void *server_param);

/** 
 * Helper function used to create and return a string, which is a copy of the
 * string s but in which all alphabetical characters are lower case
 *
 * @param s string with characters in lower and/or upper case
 * @return string with characters in lower case
 */
char *convert_to_lowercase(char *s);

/**
 * Helper function used for showing fatal error messages on stderr
 */
void print_error_exit();

/**
 * Checks if the string corresponds to a valid ip address
 *
 * @param s contains the ip address
 * @return 1 if s is valid, 0 otherwise
 */
int is_valid_ip(char *s);

/**
 * Checks if the s is a valid hostname
 *
 * @param s contains the hostname of the server to connect to
 * @return 1 if s is a valid hostname, 0 otherwise
 */
int is_valid_hostname(char *s);

/**
 * Checks if label is longer than maximum allowed
 *
 * @param number of chars between the last dot (or beginning) and now
 * @return 1 if it longer than allowed, 0 otherwise
 */
int is_label_longer_than_allowed(u_int32_t label_size);

/**
 * Checks if c is a valid hostname character
 *
 * @param c character to check
 * @return 1 if c is a valid hostname char, 0 otherwise
 */
int is_valid_hostname_char(char c);

/**
 * Checks the command line arguments, validates them, and returns the mode
 * chosen for the current program.
 *
 * @param argc number of parameters (including executable name)
 * @param argv array of command line parameters
 * @return the mode to use the program (CLIENT or SERVER)
 */
int handle_input(int argc, char *argv[]);

/**
 * Utility function to aid in debugging of the addrinfo structure returned by
 * the call to getadrinfo. It prints all the IP address in the addrinfo chain.
 *
 * @param  addrinfo structure returned by getaddrinfo()
 */
void iterate_over_results(struct addrinfo *results);

/**
 * Checks if s is a valid port number (between 1024 and 49151)
 *
 * @param s port number
 * @return 1 if it is a valid port, 0 otherwise
 */
int is_valid_port(char *s);

#endif /* ifndef GUARD_COMMON_H */
