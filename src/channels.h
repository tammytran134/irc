#ifndef CHIRC_CHANNELS_H_
#define CHIRC_CHANNELS_H_

#include <pthread.h>
#include <stdbool.h>

#include "uthash.h"

/* A hash table for clients' information in a channel */
typedef struct channel_client
{
    char *hostname; /* key */
    char *mode;     /* value */
    UT_hash_handle hh;
} channel_client_t;

/* Add client to channel */
void add_channel_client(char *hostname, channel_client_t **clients,
                        bool is_oper);

/* Remove client from channel */
void remove_channel_client(char *hostname, channel_client_t **clients);

/* Check if channel contains client */
bool contains_client(char *hostname, channel_client_t **clients);

/* Get client's information in channel */
channel_client_t* get_channel_client(char *hostname, 
                                    channel_client_t **clients);

/* A hash table for channels on the server */
typedef struct channel_hb
{
    char *channel_name; /* key */
    channel_client_t *channel_clients;
    UT_hash_handle hh;
    pthread_mutex_t lock;
} channel_hb_t;

/* Add channel to server */
void add_channel(char *channel_name, channel_hb_t **channels);
/* Remove channel from server */
void remove_channel(char *channel_name, channel_hb_t **channels);
/* Get number of channels on server */
unsigned int count_channels(channel_hb_t **channels);
/* Get number of clients of a channel */
unsigned int count_channel_clients(channel_client_t **channel_clients);
/* Get channel information from channel name */
channel_hb_t *get_channel_info(char *channel_name, channel_hb_t **channels);

#endif