#ifndef CHIRC_SERVER_H_
#define CHIRC_SERVER_H_

#include "uthash.h"
#include "clients.h"
#include "channels.h"

#define MAX_PARAMS 15
#define MAX_MSG_LEN 512
#define MAX_STR_LEN 1024
#define MAX_BUF_LEN 100
#define INCR 1
#define DECR 2
#define UNKNOWN 3
#define KNOWN 4

#define OPERATOR_MODE "+o"

typedef struct irc_oper
{
    char *hostname; /* key */
    char *mode;     /* value */
    UT_hash_handle hh;
} irc_oper_t;

typedef struct irc_operator
{
    int num_oper;
    irc_oper_t *irc_oper;
    // pthread_mutex_t lock;
} irc_operator_t;

/* A "server context" struct that contains information that 
 * needs to be shared amongst all the worker threads
 */
typedef struct server_ctx
{
    unsigned int num_connections;
    unsigned int num_unknown_connections;
    client_info_t *clients_hashtable;
    nick_hb_t *nicks_hashtable;
    channel_hb_t *channels_hashtable;
    irc_operator_t *irc_operators_hashtable;
    char *password;
    pthread_mutex_t lock;
    pthread_mutex_t channels_lock;
    pthread_mutex_t clients_lock;
    pthread_mutex_t nicks_lock;
    pthread_mutex_t operators_lock;
} server_ctx_t;

typedef struct worker_args
{
    int socket;
    char *client_hostname;
    /* ADDED: We need to pass the server context to the worker thread */
    server_ctx_t *ctx;
} worker_args_t;

typedef struct connection_info
{
    int client_socket;
    char *server_hostname;
    char *client_hostname;
    bool registered;
} connection_info_t;

/* Update number of known or unknown connections in server context object */
void change_connection(server_ctx_t *ctx, int mode, int operator);

/* Add client to server context object's clients hash table */
void server_add_client(server_ctx_t *ctx, client_info_t *client);

/* Remove client from server context object's clients hash table */
void server_remove_client(server_ctx_t *ctx, char *hostname);

/* Add nick to server context object's nicks hash table */
void server_add_nick(server_ctx_t *ctx, char *nick, char *hostname);

/* Remove nick from sever context object's nicks and hash table */
void server_remove_nick(server_ctx_t *ctx, char *nick);

/* Add client to channel in server context object's channels hash table */
void server_add_chan_client(channel_hb_t *channel, char *hostname,
                            bool is_oper);

/* Remove client from channel in server context object's channels hash table */
void server_remove_chan_client(channel_hb_t *channel, char *hostname);

/* Add channel to server context object's channels hash table */
void server_add_channel(server_ctx_t *ctx, char *channel_name);

/* Remove channel from server context object's channels hash table */
void server_remove_channel(server_ctx_t *ctx, char *channel_name);

void server_send_chan_client(channel_client_t *clients, char *msg,
                             server_ctx_t *ctx);

void send_final(client_info_t *receiver, char *msg);

bool add_irc_operator(irc_oper_t **irc_opers, char *hostname, char *mode);

void server_add_irc_operator(server_ctx_t *ctx, char *hostname, char *mode);

#endif