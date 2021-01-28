#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clients.h"

/* Functions for nick_hb_t hash table */
void add_nick(char *nick, int client_socket, nick_hb_t **nicks)
{
    /* Add nick entry with client socket as key to nicks hash table -> Void */
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
    /* Remove nick entry from nicks hash table with given nick -> Void */
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
    /* Add client struct to clients' hash table -> Void */
    pthread_mutex_init(&c->lock, NULL);
    HASH_ADD_INT(*clients, client_socket, c);
}

void remove_client(int client_socket, client_info_t **clients)
{
    /* Remove client from hashtable of clients on the server with
     * client socket and pointer to clients hash table's pointer.
     */
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
    /* Return pointer to client struct with client socket
     * and pointer to clients hash table's pointer.
     */
    client_info_t *result;
    HASH_FIND_INT(*clients, &client_socket, result);
    return result;
}

client_info_t *get_client_w_nick(
    char *nick,
    client_info_t **clients,
    nick_hb_t **nicks)
{
    /* Return pointer to client struct with client's nick,
     * pointer to clients hash table's pointer, and pointer
     * to nicks hash table's pointer.
     */
    nick_hb_t *nick_i;
    HASH_FIND_STR(*nicks, nick, nick_i);
    if (nick_i != NULL)
    {
        return get_client_info(nick_i->client_socket, clients);
    }

    return NULL;
}

bool has_entered_NICK(int client_socket, client_info_t **clients)
{
    /* Returns whehter user has entered NICK command 
     * with client socket and pointer to clients hash table's pointer.
     */
    client_info_t *client = get_client_info(client_socket, clients);
    return client != NULL && client->info.nick != NULL;
}

bool has_entered_USER(int client_socket, client_info_t **clients)
{
    /* Returns whehter user has entered USER command 
     * with client socket and pointer to clients hash table's pointer.
     */
    client_info_t *client = get_client_info(client_socket, clients);
    return client != NULL && client->info.username != NULL;
}

int count_users(client_info_t **clients)
{
    /* Return the number of clients on server with pointer to
     * clients hash table's pointer.
     */
    int num_of_users = 0;
    client_info_t *client;
    for (client = *clients; client != NULL; client = client->hh.next)
    {
        if ((client->info.nick != NULL) && (client->info.username != NULL))
        {
            num_of_users++;
        }
    }
    return num_of_users;
}