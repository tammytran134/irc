#ifndef CHIRC_CHANNELS_H_
#define CHIRC_CHANNELS_H_

#include <pthread.h>

#include "uthash.h"

/* A hash table for clients' information in a channel */
typedef struct channel_client {
    char *hostname; /* key */
    char *mode; /* value */
    pthread_mutex_t lock;
    UT_hash_handle hh;
} channel_client_t;

/* Add client to channel */
void add_channel_client(char *hostname, char *mode, channel_client_t **clients);
/* Remove client from channel */
void remove_channel_client(char *hostname, channel_client_t **clients);

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

unsigned int count_channels(channel_hb_t **channels);

unsigned int count_channel_clients(channel_client_t **channel);

channel_hb_t *get_channel_info(char *channel_name, channel_hb_t **channels);

#endif