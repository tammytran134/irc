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
    pthread_mutex_lock(&ctx->clients_lock);
    add_client(client, &ctx->clients_hashtable);
    pthread_mutex_unlock(&ctx->clients_lock);
}

void server_remove_client(server_ctx_t *ctx, int client_socket)
{
    /* Remove client from server context object's clients hash table */
    pthread_mutex_lock(&ctx->clients_lock);
    remove_client(client_socket, &ctx->clients_hashtable);
    pthread_mutex_unlock(&ctx->clients_lock);
}

void server_add_nick(server_ctx_t *ctx, char *nick, int client_socket)
{
    /* Add nick to server context object's nicks hash table */
    pthread_mutex_lock(&ctx->nicks_lock);
    add_nick(nick, client_socket, &ctx->nicks_hashtable);
    pthread_mutex_unlock(&ctx->nicks_lock);
}

void server_remove_nick(server_ctx_t *ctx, char *nick)
{
    /* Remove nick from server context object's nicks and hash table */
    pthread_mutex_lock(&ctx->nicks_lock);
    remove_nick(nick, &ctx->nicks_hashtable);
    pthread_mutex_unlock(&ctx->nicks_lock);
}

void server_add_chan_client(channel_hb_t *channel, char *nick, 
                            bool is_oper)
{
    /* Add channel client in server context object's channels hash table */
    pthread_mutex_lock(&channel->lock);
    add_channel_client(nick, &channel->channel_clients, is_oper);
    pthread_mutex_unlock(&channel->lock);
}

void server_remove_chan_client(channel_hb_t *channel, char *nick)
{
    /* Remove channel client in server context object's channels hash table */
    pthread_mutex_lock(&channel->lock);
    remove_channel_client(nick, &channel->channel_clients);
    pthread_mutex_unlock(&channel->lock);
}

void server_add_channel(server_ctx_t *ctx, char *channel_name)
{
    /* Add channel to server context object's channels hash table */
    pthread_mutex_lock(&ctx->channels_lock);
    add_channel(channel_name, &ctx->channels_hashtable);
    pthread_mutex_unlock(&ctx->channels_lock);
}

void server_remove_channel(server_ctx_t *ctx, char *channel_name)
{
    /* Remove channel from server context object's channels hash table */
    pthread_mutex_lock(&ctx->channels_lock);
    remove_channel(channel_name, &ctx->channels_hashtable);
    pthread_mutex_unlock(&ctx->channels_lock);
}

void send_final(client_info_t *receiver, char *msg)
{
    /* Wrapper function to send message to a client socket */
    pthread_mutex_lock(&receiver->lock);
    send(receiver->client_socket, msg, strlen(msg), 0);
    pthread_mutex_unlock(&receiver->lock);
}

void server_send_chan_client(channel_client_t *clients, char *msg,
                             server_ctx_t *ctx)
{
    channel_client_t *client = NULL;
    char *nick;
    for (client = clients; client != NULL; client = client->hh.next)
    {
        nick = client->nick;
        printf("server_send_chan_client: %s\n", nick);
        client_info_t *recvr = get_client_w_nick(nick, &ctx->clients_hashtable,
                                                &ctx->nicks_hashtable);
        if(recvr == NULL)
        {
            printf("recvr is NULL\n");
        }
        send_final(recvr, msg);
        printf("sever_send_chan_client: sent final message\n");
    }
}

bool add_irc_operator(irc_oper_t **irc_opers, char *nick, char *mode)
{
    irc_oper_t *irc_oper;
    HASH_FIND_STR(*irc_opers, nick, irc_oper);
    if (irc_oper == NULL)
    {
        irc_oper = malloc(sizeof(irc_oper_t));
        irc_oper->nick = malloc(sizeof(char) * strlen(nick));
        irc_oper->mode = malloc(sizeof(char) * strlen(mode));
        strcpy(irc_oper->nick, nick);
        strcpy(irc_oper->mode, mode);
        HASH_ADD_STR(*irc_opers, nick, irc_oper);
        return true;
    }
    else
    {
        return false;
    }
}

void server_add_irc_operator(server_ctx_t *ctx, char *nick, char *mode)
{
    pthread_mutex_lock(&ctx->operators_lock);
    if (add_irc_operator(&ctx->irc_operators_hashtable->irc_oper,
                         nick, mode))
    {
        ctx->irc_operators_hashtable->num_oper++;
    }
    pthread_mutex_unlock(&ctx->operators_lock);
}

void server_close_socket (server_ctx_t *ctx, int client_socket)
{
    pthread_mutex_lock(&ctx->lock);
    close(client_socket);
    pthread_mutex_unlock(&ctx->lock);
}
