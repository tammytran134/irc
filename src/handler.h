#ifndef CHIRC_HANDLER_H_
#define CHIRC_HANDLER_H_

#include "uthash.h"
#include <stdbool.h>

#define MAX_PARAMS  15
#define MAX_MSG_LEN 512
#define MAX_STR_LEN 1024

typedef struct msg
{
    char *msg;
    int counter;
    bool NICK;
    bool USER;
} msg_t;

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

void add_client(client_info *c, client_info **clients);

client_info* get_client_info(char *hostname, client_info **clients);

typedef struct cmd {
    char* command;
    int numParams;
    char *params[MAX_PARAMS];
} cmd_t;

/* IRC server's message handler helper functions */
bool sameStr(char *s1, char *s2);
msg_t recv_msg(char *buf, msg_t rmsg, client_info **clients,
    int client_socket, char *clientHostname, char *serverHostname);
cmd_t parse_msg(char *buffer);
void exec_msg(int clientSocket, client_info **clients, char *clientHostname, char *serverHostname, cmd_t msg);

#endif