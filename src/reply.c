#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>

#include "reply.h"

void reply_error(char *cmd, char *reply_code, connection_info_t *connection, client_info_t *client) 
{
    char reply_msg[MAX_LEN_STR];
    if (strcmp(reply_code, ERR_NOTREGISTERED) == 0)
    {
        server_reply(":You have not registered", reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_UNKNOWNCOMMAND) == 0)
    {
        sprintf (reply_msg, "%s :Unknown command", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NONICKNAMEGIVEN) == 0)
    {
        server_reply(":No nickname given", reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NICKNAMEINUSE) == 0)
    {
        sprintf (reply_msg, "%s :Nickname is already in use", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NEEDMOREPARAMS) == 0)
    {
        sprintf (reply_msg, "%s :Not enough parameters", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_ALREADYREGISTRED) == 0)
    {
        server_reply(":Unauthorized command (already registered)", reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_PASSWDMISMATCH) == 0)
    {
        server_reply(":Password incorrect", reply_code, connection, client);
    }
    else {
        return;
    }
    
}

void server_reply(char *msg, char *reply_code, connection_info_t *connection, client_info_t *client)
{
    char reply_msg[MAX_LEN_STR];
    char *client_hostname = connection->registered ? connection->client_hostname : "*";
    sprintf (reply_msg, ":%s %s %s %s\r\n", connection->server_hostname, reply_code, client_hostname, msg);
    //send(connection->client_socket, reply_msg, strlen(reply_msg), 0);
    send_final(NULL, client, connection, reply_msg, SERVER_REPLY);
}


//void relay_reply() {

//}

void reply_welcome(user_info_t user_info, connection_info_t *connection, client_info_t *client)
{
    /* Send RPL_WELCOME */
    char welcome_msg[MAX_LEN_STR];
    sprintf(welcome_msg,
            ":Welcome to the Internet Relay Network %s!%s@%s",
            user_info.nick,
            user_info.username,
            connection->client_hostname);
    server_reply(welcome_msg, RPL_WELCOME, connection, client);
    /* Send RPL_YOURHOST */
    char host_msg[MAX_LEN_STR];
    sprintf(host_msg,":Your host is %s, running version %d",
            connection->server_hostname, VERSION);
    server_reply(host_msg, RPL_YOURHOST, connection, client);
    /* Send RPL_CREATED */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char create_msg[MAX_LEN_STR];
    sprintf(create_msg,":This server was created %d-%02d-%02d %02d:%02d:%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    server_reply(create_msg, RPL_CREATED, connection, client);
    /* Send RPL_MYINFO replies */
    char info_msg[MAX_LEN_STR];
    sprintf(info_msg,":<servername> <version> ao mtov",
            connection->server_hostname, VERSION);
    server_reply(info_msg, RPL_MYINFO, connection, client);
}