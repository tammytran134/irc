#ifndef CHIRC_SERVER_H_
#define CHIRC_SERVER_H_

#include "uthash.h"
#include "clients.h"
#include "channels.h"

#define MAX_PARAMS  15
#define MAX_MSG_LEN 512
#define MAX_STR_LEN 1024
#define MAX_BUF_LEN 100

typedef struct irc_oper
{
    char *hostname; /* key */
    char *mode; /* value */
    UT_hash_handle hh;
} irc_oper_t;

typedef struct irc_operator
{
    int num_oper;
    irc_oper_t *irc_oper;
} irc_operator_t;

/* A "server context" struct that contains information that 
 * needs to be shared amongst all the worker threads
 */
typedef struct server_ctx
{
    unsigned int num_connections;
    // pthread_mutex_t lock;
    client_info_t *clients_hashtable;
    nick_hb_t *nicks_hashtable;
    channel_hb_t *channels_hashtable;
    irc_operator_t *irc_operators_hashtable;
    char *password;
} server_ctx_t;

typedef struct worker_args
{
    int socket;
    char *client_hostname;
    /* ADDED: We need to pass the server context to the worker thread */
    server_ctx_t *ctx;
} worker_args_t;


#endif