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

//void join_channel(server_info *ctx, *client)
//{
// call function from channels.h
// claim mutex
//    add_channel_client(hostname, mode, &(ctx->channels_hashtable));
//}