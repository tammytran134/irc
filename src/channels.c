#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "channels.h"

void add_channel_client(char *nick, char *mode, channel_client_t **clients)
{
    /* Add client to channel */
    channel_client_t *client;
    HASH_FIND_STR(*clients, nick, client);
    if (client == NULL)
    {
        client = malloc(sizeof(channel_client_t));
        client->nick = malloc(sizeof(char) * strlen(nick));
        strcpy(client->nick, nick);
        client->mode = malloc(sizeof(char) * strlen(mode));
        strcpy(client->mode, mode);
        HASH_ADD_STR(*clients, nick, client);
    }
}

/* Remove client from channel */
void remove_channel_client(char *nick, channel_client_t **clients)
{
    /* Remove client from channel */
    channel_client_t *client;
    HASH_FIND_STR(*clients, nick, client);
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

/* Remove channel from server */
void remove_channel(char *channel_name, channel_hb_t **channels);