#ifndef CHIRC_SERVER_H_
#define CHIRC_SERVER_H_

#include "uthash.h"
#include <stdbool.h>

#define MAX_PARAMS  15
#define MAX_MSG_LEN 512
#define MAX_STR_LEN 1024
#define MAX_BUF_LEN 100

/* This struct holds information about a user's registration information */
typedef struct nick_info {
    char *nick;
    char *username;
    char *realname;
} nick_info_t;


/* This struct is a hashtable whose key is the client's hostname
 * and value is the nick_info_t struct belonging to that client
 * so that we can uniquely identify each client and their registered info
 */
typedef struct client_info
{
    char *hostname; /* key for hashtable */
    nick_info_t info; /* value for hashtable */
    UT_hash_handle hh;  /* makes this struct hashable */
} client_info_t;


/* add new client to hashtable of clients on the server */
void add_client(client_info_t *c, client_info_t **clients);

/* get information about a client 
 * from the hashtable of clients on the server 
 * using the client's host name 
 */
client_info_t* get_client_info(char *hostname, client_info_t **clients);

/* A "server context" struct that contains information that 
 * needs to be shared amongst all the worker threads
 */
typedef struct server_ctx
{
    unsigned int num_connections;
    pthread_mutex_t lock;
    client_info_t *clients_hashtable;
} server_ctx_t;

typedef struct worker_args
{
    int socket;
    char *client_hostname;
    /* ADDED: We need to pass the server context to the worker thread */
    server_ctx_t *ctx;
} worker_args_t;


#endif