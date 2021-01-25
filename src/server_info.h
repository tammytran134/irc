#ifndef CHIRC_SERVER_H_
#define CHIRC_SERVER_H_

#include "uthash.h"
#include "clients.h"

#define MAX_PARAMS  15
#define MAX_MSG_LEN 512
#define MAX_STR_LEN 1024
#define MAX_BUF_LEN 100

/* A "server context" struct that contains information that 
 * needs to be shared amongst all the worker threads
 */
typedef struct server_ctx
{
    unsigned int num_connections;
    // pthread_mutex_t lock;
    client_info_t *clients_hashtable;
    nick_hb_t *nicks_hashtable;
} server_ctx_t;

typedef struct worker_args
{
    int socket;
    char *client_hostname;
    /* ADDED: We need to pass the server context to the worker thread */
    server_ctx_t *ctx;
} worker_args_t;


#endif