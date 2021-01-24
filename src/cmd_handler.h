#ifndef CHIRC_CMD_HANDLER_H_
#define CHIRC_CMD_HANDLER_H_

#include <stdbool.h>
#include "msg_handler.h"

typedef int (*handler_function)(cmd_t);

int handler_NICK(cmd_t cmd);
int handler_USER(cmd_t cmd);
int handler_QUIT(cmd_t cmd);
int handler_JOIN(cmd_t cmd);
int handler_PRIVMSG(cmd_t cmd);
int handler_NOTICE(cmd_t cmd);
int handler_LIST(cmd_t cmd);
int handler_MODE(cmd_t cmd);
int handler_OPER(cmd_t cmd);
int handler_PONG(cmd_t cmd);
int handler_LUSERS(cmd_t cmd);

typedef struct handler_entry
{
    char *name;
    handler_function func;
} handler_entry_t;

#define NICK_PAM 1
#define USER_PAM 4             


#define LIST_PAM_MIN 0             
#define LIST_PAM_MAX 1   

#define MODE_PAM 3    
#define OPER_PAM 2                 

#endif


