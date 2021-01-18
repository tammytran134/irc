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

message parse_msg(char *msgBuffer)
{
    /* Parse command from message buffer into command struct */
    char *token;
    char *rest = msgBuffer;

    message parsedMsg = {"", {}};
    int counter = 0;
    while (token = strtok_r(rest, " ", &rest))
    {
        if (counter == 0)
            parsedMsg.command = token;
        else
            parsedMsg.params[counter - 1] = token;
        counter++;
    }

    return parsedMsg;
}

void exec_msg(int clientSocket, char *clientHostname, char *serverHostname, message msg)
{
    /* Execute parsed message */
    char *replyMsg;
    char *replyCode;
    if (sameStr(msg.command, "NICK"))
    {
        char *nick = msg.params[0];
        client_info *new_client = malloc(sizeof(client_info));
        strcpy(new_client->info.nick, nick);
        strcpy(new_client->info.realname, NULL);
        strcpy(new_client->info.username, nick);
        add_client(new_client);
    }
    else if (samesStr(msg.command, "USER")) // USER user01 * * :John Doe
    {
        client_info *client = get_client_info(clientHostname);
        char *username = msg.params[0];
        char *realname = msg.params[3];
        strcpy(replyCode, RPL_WELCOME);
        char *replyMsg;
        sprintf(replyMsg,
                "%s %s %s :Welcome to the Internet Relay Network %s!%s@%s\r\n",
                serverHostname,
                replyCode,
                client->info.nick,
                client->info.nick,
                username,
                clientHostname);
        send(clientSocket, replyMsg, strlen(replyMsg), 0);
    }

    return;
}