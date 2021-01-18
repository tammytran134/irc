#ifndef CHIRC_HANDLER_H_
#define CHIRC_HANDLER_H_

#include "uthash.h"
#include <stdbool.h>

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

void add_client(client_info *c, client_info *clients_hashtable);

client_info* get_client_info(char *hostname, client_info *clients_hashtable);

typedef struct cmd {
    char* command;
    char* params[15];
} cmd_t;

/* IRC server's message handler helper functions */
bool sameStr(char *s1, char *s2);
cmd_t parse_msg(char *buffer);
void exec_msg(int clientSocket, char *clientHostname, char *serverHostname, cmd_t msg);

#endif