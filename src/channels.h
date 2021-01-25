#ifndef CHIRC_CHANNELS_H_
#define CHIRC_CHANNELS_H_

#include <pthread.h>

#include "uthash.h"

/* A hash table for clients' information in a channel */
typedef struct channel_client {
    char *nick; /* key */
    char *mode; /* value */
    UT_hash_handle hh;
} channel_client_t;

/* Add client to channel */
void add_channel_client(char *nick, char *mode, channel_client_t **clients);
/* Remove client from channel */
void remove_channel_client(char *nick, channel_client_t **clients);

/* A hash table for channels on the server */
typedef struct channel_hb {
    char *channel_name;  /* key */
    channel_client_t *channel_clients;
    UT_hash_handle hh;
} channel_hb_t;

/* Add channel to server */
void add_channel(char *channel_name, channel_hb_t **channels);
/* Remove channel from server */
void remove_channel(char *channel_name, channel_hb_t **channels);

#endif