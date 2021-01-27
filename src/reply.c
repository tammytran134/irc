#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>

#include "reply.h"

void reply_error(char *cmd, char *reply_code,
                 connection_info_t *connection, client_info_t *client)
{
    char reply_msg[MAX_LEN_STR];
    if (strcmp(reply_code, ERR_NOTREGISTERED) == 0)
    {
        server_reply(":You have not registered", reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_UNKNOWNCOMMAND) == 0)
    {
        sprintf(reply_msg, "%s :Unknown command", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NONICKNAMEGIVEN) == 0)
    {
        server_reply(":No nickname given", reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NICKNAMEINUSE) == 0)
    {
        sprintf(reply_msg, "%s :Nickname is already in use", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NEEDMOREPARAMS) == 0)
    {
        sprintf(reply_msg, "%s :Not enough parameters", cmd);
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
    else if (strcmp(reply_code, ERR_NORECIPIENT) == 0)
    {
        sprintf(reply_msg, ":No recipient given (%s)", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NOTEXTTOSEND) == 0)
    {
        server_reply(":No text to send", reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NOSUCHNICK) == 0)
    {
        sprintf(reply_msg, "%s :No such nick/channel", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_CANNOTSENDTOCHAN) == 0)
    {
        sprintf(reply_msg, "%s :Cannot send to channel", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_NOSUCHCHANNEL) == 0)
    {
        sprintf(reply_msg, "%s :No such channel", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_CHANOPRIVSNEEDED) == 0)
    {
        sprintf(reply_msg, "%s :You're not channel operator", cmd);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else
    {
        return;
    }
}

void reply_error_mult(char *cmd1, char *cmd2, char *reply_code,
                      connection_info_t *connection, client_info_t *client)
{
    char reply_msg[MAX_LEN_STR];
    if (strcmp(reply_code, ERR_UNKNOWNMODE) == 0)
    {
        sprintf(reply_msg, "%c :is unknown mode char to me for #%s",
                cmd1[1],
                cmd2);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else if (strcmp(reply_code, ERR_USERNOTINCHANNEL) == 0)
    {
        sprintf(reply_msg, "%s #%s :They aren't on that channel",
                cmd1, cmd2);
        server_reply(reply_msg, reply_code, connection, client);
    }
    else
    {
        return;
    }
}

void server_reply(char *msg, char *reply_code, connection_info_t *connection, client_info_t *client)
{
    char reply_msg[MAX_LEN_STR];
    char *client_nickname;
    if (client->info.nick != NULL) 
    {
        client_nickname = client->info.nick; 
    }
    else 
    {
        client_nickname = "*"; 
    }
    sprintf(reply_msg, ":%s %s %s %s\r\n", connection->server_hostname, reply_code, client_nickname, msg);
    //send(connection->client_socket, reply_msg, strlen(reply_msg), 0);
    send_final(client, reply_msg);
}

void relay_reply(char *msg, connection_info_t *connection,
                 client_info_t *sender, client_info_t *receiver)
{
    char reply_msg[MAX_LEN_STR];
    sprintf(reply_msg, ":%s!%s@%s %s\r\n", sender->info.nick, sender->info.username, connection->client_hostname, msg);
    send_final(receiver, reply_msg);
}

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
    sprintf(host_msg, ":Your host is %s, running version %s",
            connection->server_hostname, VERSION);
    server_reply(host_msg, RPL_YOURHOST, connection, client);
    /* Send RPL_CREATED */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char create_msg[MAX_LEN_STR];
    sprintf(create_msg, ":This server was created %d-%02d-%02d %02d:%02d:%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    server_reply(create_msg, RPL_CREATED, connection, client);
    /* Send RPL_MYINFO replies */
    char info_msg[MAX_LEN_STR];
    sprintf(info_msg, "%s %s ao mtov",
            connection->server_hostname, VERSION);
    server_reply(info_msg, RPL_MYINFO, connection, client);
}