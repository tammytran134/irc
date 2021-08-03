#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "channels.h"

void add_channel_client(char *hostname, channel_client_t **clients,
                        bool is_oper)
{
    /* Add client to channel */
    channel_client_t *client;
    HASH_FIND_STR(*clients, hostname, client);
    if (client == NULL)
    {
        client = malloc(sizeof(channel_client_t));
        client->hostname = malloc(sizeof(char) * strlen(hostname));
        strcpy(client->hostname, hostname);
        client->mode = malloc(sizeof(char) * 2);
        strcpy(client->mode, is_oper ? "+o" : "-o");
        HASH_ADD_STR(*clients, hostname, client);
    }
}

void remove_channel_client(char *hostname, channel_client_t **clients)
{
    /* Remove client from channel */
    channel_client_t *client;
    HASH_FIND_STR(*clients, hostname, client);
    if (client != NULL)
        HASH_DELETE(hh, *clients, client);
}

channel_client_t* get_channel_client(char *hostname, 
                                    channel_client_t **clients)
{
    /* Get client's information in channel */
    channel_client_t *client;
    HASH_FIND_STR(*clients, hostname, client);
    return client;
}

void add_channel(char *name, channel_hb_t **channels)
{
    /* Add channel to server */
    channel_hb_t *channel;
    HASH_FIND_STR(*channels, name, channel);
    if (channel == NULL)
    {
        channel = malloc(sizeof(channel_hb_t));
        channel->channel_clients = NULL;
        channel->channel_name = malloc(sizeof(char) * strlen(name));
        strcpy(channel->channel_name, name);
        pthread_mutex_init(&channel->lock, NULL);
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
        pthread_mutex_destroy(&channel->lock);
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

unsigned int count_channel_clients(channel_client_t **channel_clients)
{
    /* Return number of clients on a channel */
    return HASH_COUNT(*channel_clients);
}

unsigned int count_channels(channel_hb_t **channels)
{
    /* Get number of channels on server */
    return HASH_COUNT(*channels);
}

channel_hb_t *get_channel_info(char *channel_name, channel_hb_t **channels)
{
    /* Return pointer to channel with given key (channel_name) */
    channel_hb_t *result;
    HASH_FIND_STR(*channels, channel_name, result);
    return result;
}