#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>

#include "server_info.h"

/* Functions related to thread/mutex */
void *service_single_client(void *args) {
    worker_args_t *wa;
    server_ctx_t *ctx;
    client_info_t *clients_hashtable;
    int client_socket;
    int numbytes;
    char buf[MAX_BUF_LEN];
    char msg[MAX_MSG_LEN];
    msg_t rmsg = {"", 0, false, false};
    rmsg.msg = msg;

    wa = (struct worker_args*) args;
    client_socket = wa->socket;
    ctx = wa->ctx;
    clients_hashtable = wa->clients_hashtable;

    pthread_detach(pthread_self());
            /* This loop continues to listen and receive message
         * until client has put in NICK and USER command */
        while (!(rmsg.nick_cmd && rmsg.user_cmd))
        {
            if ((numbytes = recv(client_socket, buf, sizeof buf, 0)) == -1)
            {
                perror("recv() failed");
                exit(1);
            }
            buf[numbytes] = '\0';
            char server_hostname[MAX_STR_LEN];
            /* Get name of host server */
            gethostname(server_hostname, sizeof server_hostname);
            char hostname[MAX_STR_LEN];
            char port[MAX_STR_LEN];
            /* Get client's hostname */
            // int result = getnameinfo((struct sockaddr *) &client_addr,//check
            //                             sin_size, 
            //                             hostname,
            //                             sizeof hostname,
            //                             port,
            //                             sizeof port, 0);
            // /* Send the data received from the buf 
            //  * to recv_msg to parse and process */
            // rmsg = recv_msg(buf, rmsg, &clients_hashtable, client_socket, 
            //                 hostname, server_hostname);
        }
    close(client_socket);
    pthread_exit(NULL);
}

/* Functions for client_info hash table */

void add_client(client_info_t *c, client_info_t **clients)
{
    /* Add client to clients' hashtable */
    HASH_ADD_STR(*clients, hostname, c);
}

client_info_t *get_client_info(char *hostname, client_info_t **clients)
{
    /* Return pointer to client with given key (hostname) */
    client_info_t *result;
    HASH_FIND_STR(*clients, hostname, result);
    return result;
}