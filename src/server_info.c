#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>

#include "server_info.h"

/* Functions related to thread/mutex */

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