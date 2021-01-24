#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "cmd_handler.h"

bool check_cmd(int input, int standard, char *operator)
{
    if (sameStr(operator, "<="))
    {
        if (input <= standard) 
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
    if (sameStr(operator, ">="))
    {
        if (input >= standard) 
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
    if (sameStr(operator, "=="))
    {
        if (input == standard)
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
}

int handler_NICK(cmd_t cmd) 
{
    if (!(check_cmd(cmd.num_params, NICK_PAM, "==")))
    {
        // error WRONGPARAM
    }
    return 0;
}

int handler_USER(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, USER_PAM, "==")))
    {
        // error WRONGPARAM
    }
    return 0;
} 

int handler_QUIT(cmd_t cmd)
{
    if ((!(check_cmd(cmd.num_params, QUIT_PAM_MAX, "<="))) || (!(check_cmd(cmd.num_params, QUIT_PAM_MIN, ">="))))
    {
        // error WRONGPARAM
    }
    return 0;
}

int handler_JOIN(cmd_t cmd)
{
    if ((!(check_cmd(cmd.num_params, JOIN_PAM_MAX, "<="))) || (!(check_cmd(cmd.num_params, JOIN_PAM_MIN, ">="))))
    {
        // error WRONGPARAM
    }
    return 0;
}

int handler_PRIVMSG(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, PRIVMSG_PAM, "==")))
    {
        // error WRONGPARAM
    }
    return 0;
}

int handler_NOTICE(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, NOTICE_PAM, "==")))
    {
        // error WRONGPARAM
    }
    return 0;
}

int handler_LIST(cmd_t cmd)
{
    if ((!(check_cmd(cmd.num_params, LIST_PAM_MAX, "<="))) || (!(check_cmd(cmd.num_params, LIST_PAM_MIN, ">="))))
    {
        // error WRONGPARAM
    }
    return 0;
}

int handler_MODE(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, MODE_PAM, "==")))
    {
        // error WRONGPARAM
    }
    return 0;
}
int handler_OPER(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, OPER_PAM, "==")))
    {
        // error WRONGPARAM
    }
    return 0;
}

int handler_PING(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, PING_PAM, "==")))
    {
        // error WRONGPARAM
    }
    return 0;
}

int handler_PONG(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, PONG_PAM, "==")))
    {
        // error WRONGPARAM
    }
    // do nothing
    return 0;
}

int handler_LUSERS(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, LUSERS_PAM, "==")))
    {
        // error WRONGPARAM
    }
    return 0;
}

void exec_cmd(cmd_t full_cmd)
{
    handler_entry_t handlers[] = {
                                    {"NICK", handler_NICK}, 
                                    {"USER", handler_USER}, 
                                    {"QUIT", handler_QUIT}, 
                                    {"JOIN", handler_JOIN},
                                    {"PRIVMSG", handler_PRIVMSG}, 
                                    {"NOTICE", handler_NOTICE}, 
                                    {"LIST", handler_LIST}, 
                                    {"MODE", handler_MODE},  
                                    {"OPER", handler_OPER},
                                    {"PING", handler_PING},  
                                    {"PONG", handler_PONG},
                                    {"LUSERS", handler_LUSERS},
                                  };
    int num_handlers = sizeof(handlers) / sizeof(handler_entry_t);
    char *cmd = full_cmd.command;
    int i;
    for (i = 0; i < num_handlers; i++) 
    {
        if (sameStr(cmd, handlers[i].name)) {
            handlers[i].func(full_cmd);
            break;
        }
    }
    if (i == num_handlers)
    {
        // throw unknown command
    }
}