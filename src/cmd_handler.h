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

/* This function compares to string
 * input: 2 strings we want to compare
 * output: return true if they are equal, otherwise false
 */
bool sameStr(char *s1, char *s2);

/* a void pointer function that includes inputs: full command 
 * we want to execute, connection_info_t struct that 
 * contains info on client's and
 * server's hostnames, the program's context object
 * output: return 0;
 * this pointer function is used to be passed into
 * the dispatch table struct
 */
typedef int (*handler_function)(cmd_t, connection_info_t*, server_ctx_t*);

/* handler function to deal with NICK command, inputs as specified in the 
 * general void pointer function above */
int handler_NICK(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with USER command, inputs as specified in the 
 * general void pointer function above */
int handler_USER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with QUIT command, inputs as specified in the 
 * general void pointer function above */
int handler_QUIT(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with JOIN command, inputs as specified in the 
 * general void pointer function above */
int handler_JOIN(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with PRIVMSG command, inputs as specified in the 
 * general void pointer function above */
int handler_PRIVMSG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with NOTICE command, inputs as specified in the 
 * general void pointer function above */
int handler_NOTICE(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with LIST command, inputs as specified in the 
 * general void pointer function above */
int handler_LIST(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with MODE command, inputs as specified in the 
 * general void pointer function above */
int handler_MODE(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with OPER command, inputs as specified in the 
 * general void pointer function above */
int handler_OPER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with PONG command, inputs as specified in the 
 * general void pointer function above */
int handler_PONG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with LUSERS command, inputs as specified in the 
 * general void pointer function above */
int handler_LUSERS(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with PART command, inputs as specified in the 
 * general void pointer function above */
int handler_PART(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* handler function to deal with WHOIS command, inputs as specified in the 
 * general void pointer function above */
int handler_WHOIS(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx);

/* This function executes a full complete command
 * It works as a dispatch table that can send each command
 * into their specific handler function
 * input: full command we want to execute, 
 * connection_info_t struct that contains info on client's and
 * server's hostnames, the program's context object
 */
void exec_cmd(cmd_t full_cmd, connection_info_t *connection, server_ctx_t *ctx);

/* struct for command dispatch table */
typedef struct handler_entry
{
    char *name; // Name of IRC command
    handler_function func; // function to deal with the IRC command
} handler_entry_t;               

#endif


