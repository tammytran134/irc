#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "channels.h"

void add_channel_client(char *hostname, char *mode, channel_client_t **clients)
{
    /* Add client to channel */
    channel_client_t *client;
    HASH_FIND_STR(*clients, hostname, client);
    if (client == NULL)
    {
        client = malloc(sizeof(channel_client_t));
        client->hostname = malloc(sizeof(char) * strlen(hostname));
        strcpy(client->hostname, hostname);
        client->mode = malloc(sizeof(char) * strlen(mode));
        strcpy(client->mode, mode);
        HASH_ADD_STR(*clients, hostname, client);
    }
}

/* Remove client from channel */
void remove_channel_client(char *hostname, channel_client_t **clients)
{
    /* Remove client from channel */
    channel_client_t *client;
    HASH_FIND_STR(*clients, hostname, client);
    if (client != NULL)
        HASH_DELETE(hh, *clients, client);
}


void add_channel(char *name, channel_hb_t **channels)
{
    /* Add channel to server */
    channel_hb_t *channel;
    HASH_FIND_STR(*channels, name, channel);
    if (channel != NULL) {
        channel = malloc(sizeof(channel_hb_t));
        channel->channel_clients = malloc(sizeof(channel_client_t));
        channel->channel_name = malloc(sizeof(char) * strlen(name));
        strcpy(channel->channel_name, name);
        HASH_ADD_STR(*channels, channel_name, channel);
    }
}

void remove_channel(char *channel_name, channel_hb_t **channels)
{
    /* Remove channel from server */
    channel_hb_t *channel;
    HASH_FIND_STR(*channels, channel_name, channel);
    if (channel != NULL)
    {
        HASH_DELETE(hh, *channels, channel);
    }
}

bool contains_client(char *hostname, channel_client_t **clients)
{
    /* Check if channel contains client */
    channel_client_t *result;
    HASH_FIND_STR(*clients, hostname, result);
    return result != NULL;
}

unsigned int count_channels(channel_hb_t **channels) {
    return HASH_COUNT(*channels);
}

unsigned int count_channel_clients(channel_client_t **channel) 
{
    /* Return number of channels clients on server */
    return HASH_COUNT(*channel);
}

channel_hb_t *get_channel_info(char *channel_name, channel_hb_t **channels)
{
    /* Return pointer to channel with given key (channel_name) */
    channel_hb_t *result;
    HASH_FIND_STR(*channels, channel_name, result);
    return result;
}