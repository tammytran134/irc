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

void remove_nick(char *nick, nick_hb_t **nicks, client_info_t **clients)
{
    /* Remove client entry from nicks hash table and clients hashtable */
    nick_hb_t *nick_to_remove;
    HASH_FIND_STR(*nicks, nick, nick_to_remove);
    if (nick_to_remove != NULL)
    {
        /* Remove from clients hashtable */
        remove_client(nick_to_remove->hostname, clients);
        HASH_DELETE(hh, *nicks, nick_to_remove);
    }
}

/* Functions for client_info hash table */
void add_client(client_info_t *c, client_info_t **clients)
{
    /* Add client to clients' hashtable */
    HASH_ADD_STR(*clients, hostname, c);
}

void remove_client(char *hostname, client_info_t **clients)
{
    /* Remove client from hashtable of clients on the server */
    client_info_t *client;
    HASH_FIND_STR(*clients, hostname, client);
    if (client != NULL)
    {
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

client_info_t *get_client_with_nick(
    char *nick,
    client_info_t **clients,
    nick_hb_t **nicks)
{
    /* Return pointer to client with given nick */
    nick_hb_t *nick_i;
    HASH_FIND_STR(*nicks, nick, nick_i);
    if(nick_i != NULL) {
        return get_client_info(nick_i->hostname, clients);
    }
    return NULL;
}

bool has_entered_NICK(char *client_hostname, client_info_t **clients) 
{
    client_info_t *client = get_client_info(client_hostname, clients);
    if (client == NULL)
    {
        return false;
    }
    else {
        if (client->info.nick == NULL) 
        {
            return false;
        }
        else {
            return true;
        }
    }
    return true;
}

bool has_entered_USER(char *client_hostname, client_info_t **clients)
{
    client_info_t *client = get_client_info(client_hostname, clients);
    if (client == NULL)
    {
        return false;
    }
    else {
        if (client->info.username == NULL) 
        {
            return false;
        }
        else {
            return true;
        }
    }
    return true;
}

bool has_registered(char *client_hostname, client_info_t **clients)
{
    if ((has_entered_NICK(client_hostname, clients)) && (has_entered_USER(client_hostname, clients)))
    {
        return true;
    }
    else 
    {
        return false;
    }
}