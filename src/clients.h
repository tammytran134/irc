#ifndef CHIRC_CLIENTS_H_
#define CHIRC_CLIENTS_H_

#include <pthread.h>
#include <stdbool.h>

#include "uthash.h"

/* This struct holds information about a user's registration information */
typedef struct user_info
{
    char *nick;
    char *username;
    char *realname;
    bool is_irc_operator;
} user_info_t;

/* This struct is a hashtable whose key is the client's hostname
 * and value is the user_info_t struct belonging to that client
 * so that we can uniquely identify each client and their registered info
 */
typedef struct client_info
{
    int client_socket;  /* key for hashtable */
    char *hostname;
    char *server_hostname;
    user_info_t info;  /* value for hashtable */
    UT_hash_handle hh; /* makes this struct hashable */
    pthread_mutex_t lock;
} client_info_t;

/* Add new client to hashtable of clients on the server */
void add_client(client_info_t *c, client_info_t **clients);
/* Remove client from hashtable of clients on the server */
void remove_client(int client_socket, client_info_t **clients);
/* Get client information from clients hashtable with hostname */
client_info_t *get_client_info(int client_socket, client_info_t **clients);

/* This struct is a hashtable of unique nicks and their corresponding hostnames
 * where the key is the user's nick and the value is the user's hostname so we
 * can ensure that nicks are unique and retrieve client's info on the server
 * with nicks (hostname value can be used as key for client_info_t hashtable).
 */
typedef struct nick_hb
{
    char *nick;     /* key */
    int client_socket; /* value (key for client_info_t) */
    UT_hash_handle hh;
} nick_hb_t;

/* Add nick entry with socket value to nicks hash table */
void add_nick(char *nick, int client_socket, nick_hb_t **nicks);
/* Remove client entry from nicks hash table and clients hashtable */
void remove_nick(char *nick, nick_hb_t **nicks);
/* Get client information from clients hashtable with nick */
client_info_t *get_client_w_nick(
    char *nick,
    client_info_t **clients,
    nick_hb_t **nicks);

bool has_entered_NICK(int client_socket, client_info_t **clients);

bool has_entered_USER(int client_socket, client_info_t **clients);

int count_users(client_info_t **clients);

#endif