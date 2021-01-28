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

/* Hash table of IRC_operators */
typedef struct irc_oper
{
    char *nick; /* key */
    char *mode;     /* value */
    UT_hash_handle hh;
} irc_oper_t;

/* Struct that has information on
 * num of IRC operators and hash table
 * of IRC operators
 */
typedef struct irc_operator
{
    int num_oper;
    irc_oper_t *irc_oper;
} irc_operator_t;

/* A "server context" struct that contains information that 
 * needs to be shared amongst all the worker threads
 * Include: hash table of clients, channels, irc operators,
 * num of known/unknown connections, and different
 * locks for different fields within the context object
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

/* struct that is passed in service_single_client function */
typedef struct worker_args
{
    int socket;
    char *client_hostname;
    server_ctx_t *ctx;
} worker_args_t;

typedef struct connection_info
{
    int client_socket;
    char *server_hostname;
    char *client_hostname;
} connection_info_t;

/* NOTE: All functions starting with server indicate that
 * they deal with mutex locks within their body.
 * Any mutex locking/unlocking is done in this module
 */

/* Update number of known or unknown connections in server context object
 * Input: context object ctx, mode of connection to be updated:
 * KNOWN or UNKNOWN, and operator: INCREMENT OR DECREASE
 * Output: void
 */
void change_connection(server_ctx_t *ctx, int mode, int operator);

/* Add client to server context object's clients hash table 
 * Input: context object for us to find clients hash table,
 * client_info_t struct to be added to the hashtable
 * Output: void
 */
void server_add_client(server_ctx_t *ctx, client_info_t *client);

/* Remove client from server context object's clients hash table 
 * Input: context object for us to find clients hash table,
 * client_socket as key to be removed from hash table
 * Output: void
 */
void server_remove_client(server_ctx_t *ctx, int client_socket);

/* Add nick to server context object's nicks hash table
 * Input: context object for us to find nicks hash table,
 * nickname to be added as key to the hashtable, client_socket
 * as value for the key
 * Output: void
 */
void server_add_nick(server_ctx_t *ctx, char *nick, int client_socket);

/* Remove nick from sever context object's nicks and hash table
 * Input: context object for us to find nicks hash table,
 * nick as key to be removed from hash table
 * Output: void
 */
void server_remove_nick(server_ctx_t *ctx, char *nick);

/* Add client to channel in server context object's channels hash table
 * Input: hash table of channels where we will add nick as new channel member
 * is_oper indicates mode of this new channel member
 * Output: void
 */
void server_add_chan_client(channel_hb_t *channel, char *nick, bool is_oper);

/* Remove client from channel in server context object's channels hash table
 * Input: hash table of channels where we will remove channel member by his nick
 * Output: void
 */
void server_remove_chan_client(channel_hb_t *channel, char *nick);

/* Add channel to server context object's channels hash table
 * Input: context object for us to find channels hash table,
 * and channel_name as new channel to be added
 * Output: void
 */
void server_add_channel(server_ctx_t *ctx, char *channel_name);

/* Remove channel from server context object's channels hash table
 * Input: context object for us to find channels hash table,
 * and channel_name as channel to be deleted
 * Output: void
 */
void server_remove_channel(server_ctx_t *ctx, char *channel_name);

/* Send messages to all clients in channel
 * Input: channel hash table where we can find members of the channel
 * msg to be sent to everyone
 * context object to have the lock
 * Output: void
 */
void server_send_chan_client(channel_client_t *clients, char *msg,
                             server_ctx_t *ctx);

/* Send messages to receiver
 * Input: message to be sent, client who receives the message
 * Output: void
 */
void send_final(client_info_t *receiver, char *msg);

/* add an irc operator to irc_operator_t struct
 * input: irc_oper_t struct that has irc_operator_t hash table struct
 * nick and mode represents key and value of new irc operator
 * Output: void
 */
bool add_irc_operator(irc_oper_t **irc_opers, char *nick, char *mode);

/* Execute add_irc_operator but with mutex lock*/
void server_add_irc_operator(server_ctx_t *ctx, char *nick, char *mode);

/* Close socket with mutex lock
 * Input: context object to get lock from, client socket to be close
 * Output: void
 */
void server_close_socket (server_ctx_t *ctx, int client_socket);

#endif