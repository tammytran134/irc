#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>

#include "server_info.h"

/* Functions related to thread/mutex */
// unknown connections, num connections, 
void change_connection(server_ctx_t *ctx, int mode, int operator)
{
    if (mode == UNKNOWN)
    {
        if (operator == INCR)
        {   
            pthread_mutex_lock(&ctx->lock);
            ctx->num_unknown_connections++;
            pthread_mutex_unlock(&ctx->lock);
        }
        else if (operator == DECR)
        {
            pthread_mutex_lock(&ctx->lock);
            ctx->num_unknown_connections--;
            pthread_mutex_unlock(&ctx->lock);
        }
        else {
            return;
        }
    }
    else if (mode == KNOWN)
    {
        if (operator == INCR)
        {   
            pthread_mutex_lock(&ctx->lock);
            ctx->num_connections++;
            pthread_mutex_unlock(&ctx->lock);
        }
        else if (operator == DECR)
        {
            pthread_mutex_lock(&ctx->lock);
            ctx->num_connections--;
            pthread_mutex_unlock(&ctx->lock);
        }
        else {
            return;
        }
    }
    else {
        return;
    }
}


//void join_channel(server_info *ctx, *client) 
//{
    // call function from channels.h
    // claim mutex
//    add_channel_client(hostname, mode, &(ctx->channels_hashtable));
//}