#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "reply.h"

void reply_error(char *cmd, char *reply_code, connection_info_t connection) 
{
    if (strcmp(reply_code, ERR_NOTREGISTERED) == 0)
    {
        send_error_reply(":You have not registered", reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_UNKNOWNCOMMAND) == 0)
    {
        char reply_msg[MAX_LEN_STR];
        sprintf (reply_msg, "%s :Unknown command", cmd);
        send_error_reply(reply_msg, reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_NONICKNAMEGIVEN) == 0)
    {
        send_error_reply(":No nickname given", reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_NICKNAMEINUSE) == 0)
    {
        char reply_msg[MAX_LEN_STR];
        sprintf (reply_msg, "%s :Nickname is already in use", cmd);
        send_error_reply(reply_msg, reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_NEEDMOREPARAMS) == 0)
    {
        char reply_msg[MAX_LEN_STR];
        sprintf (reply_msg, "%s :Not enough parameters", cmd);
        send_error_reply(reply_msg, reply_code, connection);
    }
    else {
        return;
    }
    
}

void send_error_reply(char *msg, char *reply_code, connection_info_t connection)
{
    char reply_msg[MAX_LEN_STR];
    sprintf (reply_msg, ":%s %s %s: %s\r\n", connection.server_hostname, reply_code, connection.client_hostname, msg);
    send(connection.client_socket, reply_msg, strlen(reply_msg), 0);
}