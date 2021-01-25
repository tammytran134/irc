#ifndef CHIRC_CLIENTS_H_
#define CHIRC_CLIENTS_H_

#include <pthread.h>

#include "uthash.h"

/* This struct holds information about a user's registration information */
typedef struct user_info
{
    char *nick;
    char *username;
    char *realname;
} user_info_t;

/* This struct is a hashtable of unique nicks and their corresponding hostnames
 * where the key is the user's nick and the value is the user's hostname so we
 * can ensure that nicks are unique and retrieve client's info on the server
 * with nicks (hostname value can be used as key for client_info_t hashtable).
 */
typedef struct nick_hb
{
    pthread_mutex_t lock;
    char *nick;     /* key */
    char *hostname; /* value (key for client_info_t) */
    UT_hash_handle hh;
} nick_hb_t;

/* Add nick entry with hostname value to nicks hash table */
void add_nick(char *nick, char *hostname, nick_hb_t **nicks);
/* Remove client entry from nicks hash table and clients hashtable */
void remove_nick(char *nick, nick_hb_t **nicks, client_info_t **clients);

/* This struct is a hashtable whose key is the client's hostname
 * and value is the user_info_t struct belonging to that client
 * so that we can uniquely identify each client and their registered info
 */
typedef struct client_info
{
    pthread_mutex_t lock;
    char *hostname;    /* key for hashtable */
    user_info_t info;  /* value for hashtable */
    UT_hash_handle hh; /* makes this struct hashable */
    /* TODO: socket & socket_lock so that only one can write to the socket at a
     * time.
     */
} client_info_t;

/* Add new client to hashtable of clients on the server */
void add_client(client_info_t *c, client_info_t **clients);
/* Remove client from hashtable of clients on the server */
void remove_client(char *hostname, client_info_t **clients);

/* get client information from clients hashtable with hostname */
client_info_t *get_client_info(char *hostname, client_info_t **clients);

/* get client information from clients hashtable with nick */
client_info_t *get_client_with_nick(
    char *nick,
    client_info_t **clients,
    nick_hb_t **nicks);

#endif