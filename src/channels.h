#ifndef CHIRC_CHANNELS_H_
#define CHIRC_CHANNELS_H_

#include <pthread.h>
#include <stdbool.h>

#include "uthash.h"

/* A hash table for clients' information in a channel */
typedef struct channel_client
{
    char *nick; /* key */
    char *mode;     /* value */
    UT_hash_handle hh;
} channel_client_t;

/* @Purpose: Add channel to server
 * @Params:
 * name: pointer to channel's name string
 * channels: pointer to channels hash table's pointer
 */
void add_channel_client(char *nick, channel_client_t **clients, bool is_oper);

/* @Purpose: Remove channel from server
 * @Params:
 * channel_name: pointer to channel's name string
 * channels: pointer to channels hash table's pointer
 */
void remove_channel_client(char *nick, channel_client_t **clients);

/* @Purpose: Checks if channel contains a client with given nick
 * @Params:
 * nick: pointer to target client's nick string
 * clients: pointer to channel's clients hash table's pointer
 * @Output: True if client is a member of channel, otherwise False
 */
bool contains_client(char *nick, channel_client_t **clients);

/* @Purpose: Get client's information in channel with nick
 * @Params:
 * nick: pointer to nick string
 * clients: pointer to channel's clients hash table's pointer
 * @Output: channel client struct
 */
channel_client_t* get_channel_client(char *nick, channel_client_t **clients);

/* A hash table for channels on the server */
typedef struct channel_hb
{
    char *channel_name; /* key */
    channel_client_t *channel_clients;
    UT_hash_handle hh;
    pthread_mutex_t lock;
} channel_hb_t;

/* @Purpose: Add channel to server
 * @Params:
 * name: pointer to channel's name string
 * channels: pointer to channels hash table's pointer
 */
void add_channel(char *channel_name, channel_hb_t **channels);

/* @Purpose: Remove channel from server
 * @Params:
 * channel_name: pointer to channel's name string
 * channels: pointer to channels hash table's pointer
 */
void remove_channel(char *channel_name, channel_hb_t **channels);

/* @Purpose: Count number of channels on server
 * @Params:
 * channel_name: pointer to channel's name string
 * channels: pointer to channels hash table's pointer
 * @Ouput: number of channels on server
 */
unsigned int count_channels(channel_hb_t **channels);

/* @Purpose: Count number of clients in channel
 * @Params:
 * channel_clients: pointer to channel's clients hash table's pointer
 * @Ouput: number of clients in given channel's clients hash table
 */
unsigned int count_channel_clients(channel_client_t **channel_clients);

/* @Purpose: Get channel's information from channel name
 * @Params:
 * channel_name: pointer to channel's name string
 * channels: pointer to channels hash table's pointer
 * @Ouput: Channel struct pointer
 */
channel_hb_t *get_channel_info(char *channel_name, channel_hb_t **channels);

#endif