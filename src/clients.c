#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clients.h"

/* Functions for nick_hb_t hash table */
void add_nick(char *nick, int client_socket, nick_hb_t **nicks)
{
    /* Add nick entry with hostname value to nicks hash table */
    nick_hb_t *current_nick;
    HASH_FIND_INT(*nicks, &client_socket, current_nick);
    if (current_nick != NULL)
        return;
    nick_hb_t *nick_i = malloc(sizeof(nick_hb_t));
    nick_i->nick = malloc(sizeof(char) * strlen(nick));
    strcpy(nick_i->nick, nick);
    nick_i->client_socket = client_socket;
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
    HASH_ADD_INT(*clients, client_socket, c);
}

void remove_client(int client_socket, client_info_t **clients)
{
    /* Remove client from hashtable of clients on the server */
    client_info_t *client;
    HASH_FIND_INT(*clients, &client_socket, client);
    if (client != NULL)
    {
        pthread_mutex_destroy(&client->lock);
        HASH_DELETE(hh, *clients, client);
    }
}

client_info_t *get_client_info(int client_socket, client_info_t **clients)
{
    /* Return pointer to client with given key (hostname) */
    client_info_t *result;
    HASH_FIND_INT(*clients, &client_socket, result);
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
        return get_client_info(nick_i->client_socket, clients);
    }
    printf("get_client_w_nick: nick_i is NULL\n");
    return NULL;
}

bool has_entered_NICK(int client_socket, client_info_t **clients)
{
    /* Checks if client has executed NICK command */
    client_info_t *client = get_client_info(client_socket, clients);
    return client != NULL && client->info.nick != NULL;
}

bool has_entered_USER(int client_socket, client_info_t **clients)
{
    /* Checks if client has executed USER command */
    client_info_t *client = get_client_info(client_socket, clients);
    return client != NULL && client->info.username != NULL;
}