#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clients.h"

/* Functions for nick_hb_t hash table */
void add_nick(char *nick, char *hostname, nick_hb_t **nicks)
{
    /* Add nick entry with hostname value to nicks hash table */
    nick_hb_t *current_nick;
    HASH_FIND_STR(*nicks, nick, current_nick);
    if (current_nick != NULL)
        return;
    nick_hb_t *nick_i = malloc(sizeof(nick_hb_t));
    nick_i->hostname = malloc(sizeof(char) * strlen(hostname));
    strcpy(nick_i->hostname, hostname);
    nick_i->nick = malloc(sizeof(char) * strlen(nick));
    strcpy(nick_i->nick, nick);
    HASH_ADD_STR(*nicks, nick, nick_i);
    return;
}

void remove_nick(char *nick, nick_hb_t **nicks)
{
    /* Remove client entry from nicks hash table and clients hashtable */
    nick_hb_t *nick_to_remove;
    HASH_FIND_STR(*nicks, nick, nick_to_remove);
    if (nick_to_remove != NULL)
    {
        HASH_DELETE(hh, *nicks, nick_to_remove);
    }
}

/* Functions for client_info hash table */
void add_client(client_info_t *c, client_info_t **clients)
{
    /* Add client to clients' hashtable */
    pthread_mutex_init(&c->lock, NULL);
    HASH_ADD_STR(*clients, hostname, c);
}

void remove_client(char *hostname, client_info_t **clients)
{
    /* Remove client from hashtable of clients on the server */
    client_info_t *client;
    HASH_FIND_STR(*clients, hostname, client);
    if (client != NULL)
    {
        pthread_mutex_destroy(&client->lock);
        HASH_DELETE(hh, *clients, client);
    }
}

client_info_t *get_client_info(char *hostname, client_info_t **clients)
{
    /* Return pointer to client with given key (hostname) */
    client_info_t *result;
    HASH_FIND_STR(*clients, hostname, result);
    return result;
}

client_info_t *get_client_w_nick(
    char *nick,
    client_info_t **clients,
    nick_hb_t **nicks)
{
    /* Return pointer to client with given nick */
    nick_hb_t *nick_i;
    HASH_FIND_STR(*nicks, nick, nick_i);
    if (nick_i != NULL)
    {
        return get_client_info(nick_i->hostname, clients);
    }
    return NULL;
}

bool has_entered_NICK(char *client_hostname, client_info_t **clients)
{
    /* Checks if client has executed NICK command */
    client_info_t *client = get_client_info(client_hostname, clients);
    return client != NULL && client->info.nick != NULL;
}

bool has_entered_USER(char *client_hostname, client_info_t **clients)
{
    /* Checks if client has executed USER command */
    client_info_t *client = get_client_info(client_hostname, clients);
    return client != NULL && client->info.username != NULL;
}

bool has_registered(char *client_hostname, client_info_t **clients)
{
    /* Checks if client has executed either the NICK and USER command */
    return has_entered_NICK(client_hostname, clients) &&
           has_entered_USER(client_hostname, clients);
}