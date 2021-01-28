#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "channels.h"

void add_channel_client(char *nick, channel_client_t **clients,
                        bool is_oper)
{
    /* @Purpose: Add client to channel
     * @Params:
     * nick: pointer to nick string
     * clients: pointer to channel's clients hash table's pointer
     * is_oper: whether channel client to be added is an operator
     */
    channel_client_t *client;
    HASH_FIND_STR(*clients, nick, client);
    if (client == NULL)
    {
        client = malloc(sizeof(channel_client_t));
        client->nick = malloc(sizeof(char) * strlen(nick));
        strcpy(client->nick, nick);
        client->mode = malloc(sizeof(char) * 2);
        strcpy(client->mode, is_oper ? "+o" : "-o");
        HASH_ADD_STR(*clients, nick, client);
    }
}

void remove_channel_client(char *nick, channel_client_t **clients)
{
    /* @Purpose: Remove client from channel
     * @Params:
     * nick: pointer to nick string
     * clients: pointer to channel's clients hash table's pointer
     */
    channel_client_t *client;
    HASH_FIND_STR(*clients, nick, client);
    if (client != NULL)
        HASH_DELETE(hh, *clients, client);
}

channel_client_t* get_channel_client(char *nick, 
                                    channel_client_t **clients)
{
    /* @Purpose: Get client's information in channel with nick
     * @Params:
     * nick: pointer to nick string
     * clients: pointer to channel's clients hash table's pointer
     * @Output: channel client struct
     */
    channel_client_t *client;
    HASH_FIND_STR(*clients, nick, client);
    return client;
}

void add_channel(char *name, channel_hb_t **channels)
{
    /* @Purpose: Add channel to server
     * @Params:
     * name: pointer to channel's name string
     * channels: pointer to channels hash table's pointer
     */
    channel_hb_t *channel = malloc(sizeof(*channel));
    channel->channel_clients = NULL;
    channel->channel_name = malloc(sizeof(char) * strlen(name));
    strcpy(channel->channel_name, name);
    pthread_mutex_init(&channel->lock, NULL);
    HASH_ADD_STR(*channels, channel_name, channel);
}

void remove_channel(char *channel_name, channel_hb_t **channels)
{
    /* @Purpose: Remove channel from server
     * @Params:
     * channel_name: pointer to channel's name string
     * channels: pointer to channels hash table's pointer
     */
    channel_hb_t *channel;
    HASH_FIND_STR(*channels, channel_name, channel);
    if (channel != NULL)
    {
        pthread_mutex_destroy(&channel->lock);
        HASH_DELETE(hh, *channels, channel);
    }
}

bool contains_client(char *nick, channel_client_t **clients)
{
    /* @Purpose: Checks if channel contains a client with given nick
     * @Params:
     * nick: pointer to target client's nick string
     * clients: pointer to channel's clients hash table's pointer
     * @Output: True if client is a member of channel, otherwise False
     */
    channel_client_t *result;
    HASH_FIND_STR(*clients, nick, result);
    return result != NULL;
}

unsigned int count_channel_clients(channel_client_t **channel_clients)
{
    /* @Purpose: Count number of clients in channel
     * @Params:
     * channel_clients: pointer to channel's clients hash table's pointer
     * @Ouput: number of clients in given channel's clients hash table
     */
    return HASH_COUNT(*channel_clients);
}

unsigned int count_channels(channel_hb_t **channels)
{
    /* @Purpose: Count number of channels on server
     * @Params:
     * channel_name: pointer to channel's name string
     * channels: pointer to channels hash table's pointer
     * @Ouput: number of channels on server
     */
    return HASH_COUNT(*channels);
}

channel_hb_t *get_channel_info(char *channel_name, channel_hb_t **channels)
{
    /* @Purpose: Get channel's information from channel name
     * @Params:
     * channel_name: pointer to channel's name string
     * channels: pointer to channels hash table's pointer
     * @Ouput: Channel struct pointer
     */
    channel_hb_t *result;
    HASH_FIND_STR(*channels, channel_name, result);
    return result;
}