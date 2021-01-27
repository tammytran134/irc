#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>

#include "server_info.h"

/* Functions related to thread/mutex */
void change_connection(server_ctx_t *ctx, int mode, int operator)
{
    /* Update number of known or unknown connections */
    if (mode == UNKNOWN)
    {
        if (operator== INCR)
        {
            pthread_mutex_lock(&ctx->lock);
            ctx->num_unknown_connections++;
            pthread_mutex_unlock(&ctx->lock);
        }
        else if (operator== DECR)
        {
            pthread_mutex_lock(&ctx->lock);
            ctx->num_unknown_connections--;
            pthread_mutex_unlock(&ctx->lock);
        }
        else
        {
            return;
        }
    }
    else if (mode == KNOWN)
    {
        if (operator== INCR)
        {
            pthread_mutex_lock(&ctx->lock);
            ctx->num_connections++;
            pthread_mutex_unlock(&ctx->lock);
        }
        else if (operator== DECR)
        {
            pthread_mutex_lock(&ctx->lock);
            ctx->num_connections--;
            pthread_mutex_unlock(&ctx->lock);
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}

void server_add_client(server_ctx_t *ctx, client_info_t *client)
{
    /* Add client to server context object's clients hash table */
    pthread_mutex_lock(&ctx->clients_hashtable->lock);
    add_client(client, &ctx->clients_hashtable);
    pthread_mutex_unlock(&ctx->clients_hashtable->lock);
}

void server_remove_client(server_ctx_t *ctx, char *hostname)
{
    /* Remove client from server context object's clients hash table */
    pthread_mutex_lock(&ctx->clients_hashtable->lock);
    remove_client(hostname, &ctx->clients_hashtable);
    pthread_mutex_unlock(&ctx->clients_hashtable->lock);
}

void server_add_nick(server_ctx_t *ctx, char *nick, char *hostname)
{
    /* Add nick to server context object's nicks hash table */
    pthread_mutex_lock(&ctx->nicks_hashtable->lock);
    add_nick(nick, hostname, &ctx->nicks_hashtable);
    pthread_mutex_unlock(&ctx->nicks_hashtable->lock);
}

void server_remove_nick(server_ctx_t *ctx, char *nick)
{
    /* Remove nick from server context object's nicks and hash table */
    pthread_mutex_lock(&ctx->nicks_hashtable->lock);
    remove_nick(nick, &ctx->nicks_hashtable);
    pthread_mutex_unlock(&ctx->nicks_hashtable->lock);
}

void server_add_chan_client(channel_hb_t *channel, char *hostname, char *mode)
{
    /* Add channel client in server context object's channels hash table */
    pthread_mutex_lock(&channel->lock);
    add_channel_client(hostname, mode, &channel->channel_clients);
    pthread_mutex_unlock(&channel->lock);
}

void server_remove_chan_client(channel_hb_t *channel, char *hostname)
{
    /* Remove channel client in server context object's channels hash table */
    pthread_mutex_lock(&channel->lock);
    remove_channel_client(hostname, &channel->channel_clients);
    pthread_mutex_unlock(&channel->lock);
}

void send_final(client_info_t *receiver, char *msg) 
{
    pthread_mutex_lock(&receiver->lock);
    send(receiver->client_socket, msg, strlen(msg), 0);
    pthread_mutex_lock(&receiver->lock);
}

void server_send_chan_client(channel_client_t *clients, char *msg, server_ctx_t *ctx) 
{
    channel_client_t *client = NULL;
    char *client_hostname;
    for (client=clients; client != NULL; client=clients->hh.next) 
    {
        client_hostname = client->hostname;
        client_info_t *receiver = get_client_info(client_hostname,
                                    &ctx->clients_hashtable);
        send_final(receiver, msg);
    }
}

bool add_irc_operator(irc_oper_t **irc_opers, char *hostname, char *mode)
{
    irc_oper_t *irc_oper;
    HASH_FIND_STR(*irc_opers, hostname, irc_oper);
    if (irc_oper == NULL) {
        irc_oper = malloc(sizeof(irc_oper_t));
        irc_oper->hostname = malloc(sizeof(char) * strlen(hostname));
        irc_oper->mode = malloc(sizeof(char) * strlen(mode));
        strcpy(irc_oper->hostname, hostname);
        strcpy(irc_oper->mode, mode);
        HASH_ADD_STR(*irc_opers, hostname, irc_oper);
        return true;
    }
    else 
    {
        return false;
    }

}

void server_add_irc_operator(irc_operator_t *irc_operators, char *hostname, char *mode)
{
    pthread_mutex_lock(&irc_operators->lock);
    if (add_irc_operator(&irc_operators->irc_oper, hostname, mode))
    {
        irc_operators->num_oper++;
    }
    pthread_mutex_lock(&irc_operators->lock);
}