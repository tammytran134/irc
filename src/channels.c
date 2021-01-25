#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (client != NULL) {
        HASH_DELETE(hh, *clients, client);
    }
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