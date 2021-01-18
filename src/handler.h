#ifndef CHIRC_HANDLER_H_
#define CHIRC_HANDLER_H_

#include "lib/uthash.h"

/* A HASHTABLE STORING ACTIVE NICKS' INFORMATION */
typedef struct nick_info_struct {
    char *nick;
    char *username;
    char *realname;
} nick_info;
typedef struct client_info_struct
{
    char *hostname; /* key for hashtable */
    nick_info info;
    UT_hash_handle hh;  /* makes this struct hashable */
} client_info;

client_info *clients_hashtable = NULL;

void add_client(client_info *c) {
    HASH_ADD_STR(clients_hashtable, hostname, c);
}

client_info* get_client_info(char *hostname) {
    client_info *result;
    HASH_FIND_STR(clients_hashtable, &hostname, result);
    return result;
}

typedef struct msg {
    char* command;
    char* params[15];
} message;

/* IRC server's message handler helper functions */
message parse_msg(char *buffer);
void exec_msg(int clientSocket, char *clientHostname, char *serverHostname, message msg);

#endif