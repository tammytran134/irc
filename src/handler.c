#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "handler.h"
#include "reply.h"

bool sameStr(char *s1, char *s2)
{
    return strcmp(s1, s2) == 0;
}

cmd_t parse_msg(char *msgBuffer)
{
    /* Parse command from message buffer into command struct */
    char *token;
    char *rest = msgBuffer;

    cmd_t parsedMsg = {"", {}};
    int counter = 0;
    while (token == strtok_r(rest, " ", &rest))
    {
        if (counter == 0)
            parsedMsg.command = token;
        else
            parsedMsg.params[counter - 1] = token;
        counter++;
    }

    return parsedMsg;
}

void send_welcome(
    int clientSocket,
    char *replyCode,
    char *clientHostname,
    char *serverHostname,
    char *username,
    char *nick)
{
    char *replyMsg;
    sprintf(replyMsg,
            "%s %s %s :Welcome to the Internet Relay Network %s!%s@%s\r\n",
            serverHostname,
            replyCode,
            nick,
            nick,
            username,
            clientHostname);
    send(clientSocket, replyMsg, strlen(replyMsg), 0);
    return;
}

void exec_msg(int clientSocket, char *clientHostname, char *serverHostname, cmd_t msg)
{
    /* Execute parsed message */
    char *replyMsg;
    char *replyCode;
    client_info *client = get_client_info(clientHostname);
    if (sameStr(msg.command, "NICK"))
    {
        char *nick = msg.params[0];
        if (client == NULL)
        {
            /* NICK == first command */
            client_info *new_client = malloc(sizeof(client_info));
            strcpy(new_client->info.nick, nick);
            strcpy(new_client->info.realname, NULL);
            strcpy(new_client->info.username, NULL);
            add_client(new_client);
        }
        else
        {
            /* NICK == second command */
            strcpy(client->info.nick, nick);
            if (client->info.username != NULL)
            {
                strcpy(replyCode, RPL_WELCOME);
                send_welcome(
                    clientSocket,
                    replyCode,
                    clientHostname,
                    serverHostname,
                    client->info.username,
                    client->info.nick);
            }
        }
    }
    else if (sameStr(msg.command, "USER"))
    {
        char *username;
        strcpy(username, msg.params[0]);
        char *realname;
        strcpy(realname, msg.params[3]);
        if (client != NULL)
        {
            /* USER == second name */
            strcpy(client->info.username, username);
            strcpy(client->info.realname, realname);
            if (client->info.nick != NULL)
            {
                strcpy(replyCode, RPL_WELCOME);
                send_welcome(
                    clientSocket,
                    replyCode,
                    clientHostname,
                    serverHostname,
                    username,
                    client->info.nick);
            }
        }
        else
        {
            /* USER == first command */
            client_info *new_client = malloc(sizeof(client_info));
            strcpy(new_client->info.nick, NULL);
            strcpy(new_client->info.realname, realname);
            strcpy(new_client->info.username, username);
            add_client(new_client);
        }
    }

    return;
}