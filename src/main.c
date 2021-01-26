/*
 *
 *  chirc: a simple multi-threaded IRC server
 *
 *  This module provides the main() function for the server,
 *  and parses the command-line arguments to the chirc executable.
 *
 */

/*
 *  Copyright (c) 2011-2020, The University of Chicago
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or withsend
 *  modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  - Neither the name of The University of Chicago nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software withsend specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY send OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include "log.h"
#include "msg_handler.h"
#include "server_info.h"
#include "clients.h"

void *service_single_client(void *args) {
    worker_args_t *wa;
    server_ctx_t *ctx;
    int client_socket;
    int numbytes;
    char buf[MAX_BUF_LEN];
    char msg[MAX_MSG_LEN];
    char *client_hostname;
    msg_t rmsg = {"", 0, false, false};
    rmsg.msg = msg;

    wa = (worker_args_t *) args;
    client_socket = wa->socket;
    client_hostname = wa->client_hostname;
    ctx = wa->ctx;

    pthread_detach(pthread_self());
    while ((numbytes = recv(client_socket, buf, sizeof buf, 0)) != -1)
    {
        buf[numbytes] = '\0';
        /* Get name of host server */
        char server_hostname[MAX_STR_LEN];
        gethostname(server_hostname, sizeof server_hostname);
        /* Send the data received from the buf 
         * to recv_msg to parse and process */
        connection_info_t *connection = calloc(1, sizeof(connection_info_t));
        connection->client_socket = client_socket;
        connection->server_hostname = server_hostname;
        connection->client_hostname = client_hostname;
        connection->registered = false;
        rmsg = recv_msg(buf, rmsg, ctx, connection);
    }
    /* if client's connection is unknown, change the 
     * unknown_connection field in ctx when client quits
     */
    client_info_t *clients = ctx->clients_hashtable;
    if ((has_entered_NICK(client_hostname, &clients)) || (has_entered_USER(client_hostname, &clients)))
    {
        change_connection(ctx, UNKNOWN, DECR);
    }
    close(client_socket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int opt;
    char *port = "6667", *passwd = NULL;
    char *servername = NULL, *network_file = NULL;
    int verbosity = 0;

    while ((opt = getopt(argc, argv, "p:o:s:n:vqh")) != -1)
        switch (opt)
        {
        case 'p':
            port = strdup(optarg);
            break;
        case 'o':
            passwd = strdup(optarg);
            break;
        case 's':
            servername = strdup(optarg);
            break;
        case 'n':
            if (access(optarg, R_OK) == -1)
            {
                printf("ERROR: No such file: %s\n", optarg);
                exit(-1);
            }
            network_file = strdup(optarg);
            break;
        case 'v':
            verbosity++;
            break;
        case 'q':
            verbosity = -1;
            break;
        case 'h':
            printf("Usage: chirc -o OPER_PASSWD [-p PORT] [-s SERVERNAME] [-n NETWORK_FILE] [(-q|-v|-vv)]\n");
            exit(0);
            break;
        default:
            fprintf(stderr, "ERROR: Unknown option -%c\n", opt);
            exit(-1);
        }

    if (!passwd)
    {
        fprintf(stderr, "ERROR: You must specify an operator password\n");
        exit(-1);
    }

    if (network_file && !servername)
    {
        fprintf(stderr, "ERROR: If specifying a network file, you must also specify a server name.\n");
        exit(-1);
    }

    /* Set logging level based on verbosity */
    switch (verbosity)
    {
    case -1:
        chirc_setloglevel(QUIET);
        break;
    case 0:
        chirc_setloglevel(INFO);
        break;
    case 1:
        chirc_setloglevel(DEBUG);
        break;
    case 2:
        chirc_setloglevel(TRACE);
        break;
    default:
        chirc_setloglevel(TRACE);
        break;
    }

    /* Your code goes here */
    
    /* Initialize hashtable of clients' information */
    client_info_t *clients_hashtable = NULL;
    /* Initialize hashtable of nicks' information */
    nick_hb_t *nicks_hashtable = NULL;
    /* Initialize hashtable of channels' information */
    channel_hb_t *channels_hashtable = calloc(1, sizeof(channel_hb_t));
    channel_client_t *channel_clients = NULL;
    channels_hashtable->channel_clients = channel_clients;
    /* Initialize hashtable of irc operators' information */
    irc_operator_t *irc_operators_hashtable = calloc(1, sizeof(irc_operator_t));
    irc_oper_t *irc_oper = NULL;
    irc_operators_hashtable->irc_oper = irc_oper;

    /* Initialize context object */
    server_ctx_t *ctx = calloc(1, sizeof(server_ctx_t));
    ctx->num_connections = 0;
    ctx->num_unknown_connections = 0;
    ctx->clients_hashtable = clients_hashtable;
    ctx->nicks_hashtable = nicks_hashtable;
    ctx->channels_hashtable = channels_hashtable;
    ctx->irc_operators_hashtable = irc_operators_hashtable;
    ctx->password = passwd;

    pthread_mutex_init(&ctx->lock, NULL);

    sigset_t new;
    sigemptyset (&new);
    sigaddset(&new, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &new, NULL) != 0) 
    {
        perror("Unable to mask SIGPIPE");
        exit(-1);
    }

    /* Set up socket */
    int server_socket;
    int client_socket;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage *client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_storage);
    int yes = 1;
    pthread_t worker_thread;
    worker_args_t (*wa);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &res) != 0)
    {
        perror("getaddrinfo() failed");
        exit(-1);
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((server_socket = socket(p->ai_family, 
                                    p->ai_socktype, 
                                    p->ai_protocol)) == -1)
        {
            perror("socketfailed");
            continue;
        }
        if (setsockopt(server_socket, SOL_SOCKET, 
                        SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt() failed");
            continue;
        }
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("bind() failed");
            close(server_socket);
            continue;
        }
        if (listen(server_socket, 5) == -1)
        {
            perror("listen() failed");
            close(server_socket);
        }
        break;
    }

    free(res);

    if (p == NULL)
    {
        perror("Couldn't find a socket to bind to");
        exit(-1);
    }

    while (1)
    {   
        client_addr = calloc(1, sin_size);
        client_socket = accept(server_socket, 
                                (struct sockaddr *) client_addr, 
                                &sin_size);
        change_connection(ctx, UNKNOWN, INCR);
        if (client_socket == -1) 
        {
            free(client_addr);
            perror("Could not accept() connection");
            continue;
        }

        /* Get client's hostname */
        char client_hostname[MAX_STR_LEN];
        char port[MAX_STR_LEN];
        int result = getnameinfo((struct sockaddr *) &client_addr,//check
                                    sin_size, 
                                    client_hostname,
                                    sizeof client_hostname,
                                    port,
                                    sizeof port, 0);

        wa = calloc(1, sizeof(worker_args_t));
        wa->socket = client_socket;
        wa->client_hostname = client_hostname;
        wa->ctx = ctx;

        if (pthread_create(&worker_thread, NULL, service_single_client, wa) != 0)
        {
            perror("Could not create a worker thread");
            free(client_addr);
            free(wa);
            close(client_socket);
            close(server_socket);
            return EXIT_FAILURE;
        }
    }
    close(server_socket);
    /* ADDED: Destroy the lock */
    pthread_mutex_destroy(&ctx->lock);
    free(ctx);

    return EXIT_SUCCESS;
}
