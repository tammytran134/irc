#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "reply.h"

void send_msg(char *msg, int client_socket)
{
    /* Wrapper function for sending messages to clients */
    send(client_socket, msg, strlen(msg), 0);
}

void reply_error(char *cmd, char *reply_code, connection_info_t *connection)
{
    /* Construct error message and send reply */
    char reply_msg[MAX_LEN_STR];
    if (strcmp(reply_code, ERR_NOTREGISTERED) == 0)
    {
        send_error(":You have not registered", reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_UNKNOWNCOMMAND) == 0)
    {
        sprintf(reply_msg, "%s :Unknown command", cmd);
        send_error(reply_msg, reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_NONICKNAMEGIVEN) == 0)
    {
        send_error(":No nickname given", reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_NICKNAMEINUSE) == 0)
    {
        sprintf(reply_msg, "%s :Nickname is already in use", cmd);
        send_error(reply_msg, reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_NEEDMOREPARAMS) == 0)
    {
        sprintf(reply_msg, "%s :Not enough parameters", cmd);
        send_error(reply_msg, reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_ALREADYREGISTRED) == 0)
    {
        send_error(":Unauthorized command (already registered)", reply_code, connection);
    }
    else if (strcmp(reply_code, ERR_PASSWDMISMATCH) == 0)
    {
        send_error(":Password incorrect", reply_code, connection);
    }
    else
    {
        return;
    }
}

void send_error(char *msg, char *reply_code, connection_info_t *connection)
{
    /* Send passed in error message */
    char reply_msg[MAX_LEN_STR];
    char *client_hostname = connection->registered
                                ? connection->client_hostname
                                : "*";
    sprintf(reply_msg, ":%s %s * %s: %s\r\n",
            connection->server_hostname,
            reply_code,
            client_hostname,
            msg);
    send_msg(reply_msg, connection->client_socket);
}

void reply_welcome(
    user_info_t user_info,
    char *reply_code,
    connection_info_t *connection)
{
    /* Send welcome message */
    char reply_msg[MAX_LEN_STR];
    sprintf(reply_msg,
            ":%s %s %s :Welcome to the Internet Relay Network %s!%s@%s\r\n",
            connection->server_hostname,
            reply_code, 
            user_info.nick,
            user_info.nick,
            user_info.username,
            connection->client_hostname);
    send_msg(reply_msg, connection->client_socket);
}