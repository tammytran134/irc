#ifndef CHIRC_CMD_HANDLER_H_
#define CHIRC_CMD_HANDLER_H_

#include <stdbool.h>
#include "channels.h"
#include "clients.h"
#include "server_info.h"
#include "reply.h"
#include "uthash.h"

#define NICK_PAM 1
#define USER_PAM 4             
#define JOIN_PAM 1 
#define OPER_PAM 2
#define PART_PAM 1
#define WHOIS_PAM 1

#define NUM_SERVERS 1
#define NUM_SERVICES 0

/* This struct breaks down a complete command 
 * into command and parameters
 */
typedef struct cmd {
    /* This will be the first string in the command line */
    char* command;
    /* How many params a certain command requires */
    int num_params;
    /* each element in the array will represent a parameter in order */
    char *params[MAX_PARAMS];
} cmd_t;

bool sameStr(char *s1, char *s2);

typedef int (*handler_function)(cmd_t, connection_info_t*, server_ctx_t*);

int handler_NICK(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_USER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_QUIT(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_JOIN(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_PRIVMSG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_NOTICE(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_LIST(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_MODE(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_OPER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_PONG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_LUSERS(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_PART(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);
int handler_WHOIS(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

void exec_cmd(cmd_t full_cmd, connection_info_t *connection, server_ctx_t *ctx);

typedef struct handler_entry
{
    char *name;
    handler_function func;
} handler_entry_t;               

#endif


