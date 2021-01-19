#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "handler.h"
#include "reply.h"

client_info *clients_hashtable = NULL;

void add_client(client_info *c)
{
    HASH_ADD_STR(clients_hashtable, hostname, c);
}

client_info *get_client_info(char *hostname)
{
    client_info *result;
    HASH_FIND_STR(clients_hashtable, hostname, result);
    return result;
}

bool sameStr(char *s1, char *s2)
{
    return strcmp(s1, s2) == 0;
}

void send_welcome(
    int clientSocket,
    char *replyCode,
    char *clientHostname,
    char *serverHostname,
    char *username,
    char *nick)
{
    char *replyMsg = malloc(sizeof(char) * 1024);
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

cmd_t parse_msg(char *msgBuffer)
{
    /* Parse command from message buffer into command struct */
    char *token;
    char *rest = msgBuffer;

    // printf("msgBuffer: %s\n", msgBuffer);

    cmd_t parsedMsg;
    parsedMsg.numParams = 0;
    int counter = 0;
    bool paramIsRest = false;
    while ((token = strtok_r(rest, " \t\r\n", &rest)))
    {
        // printf("counter: %d\n", counter);
        if (counter == 0)
        {
            // printf("CMD: %s\n", token);
            parsedMsg.command = malloc(sizeof(char) * strlen(token));
            strcpy(parsedMsg.command, token);
        }
        else if (counter < 15)
        {
            if (!paramIsRest)
            {
                parsedMsg.params[counter - 1] = malloc(sizeof(char) * strlen(token));
                if (token[0] == ':')
                    paramIsRest = true;
                token = strtok(token, ":");
                strcpy(parsedMsg.params[counter - 1], token);
            }
            else
            {
                char *paramSoFar = parsedMsg.params[counter - 1];
                int concatParamLen = strlen(paramSoFar) + strlen(token) + 1;
                char *concatParam = malloc(sizeof(char) * concatParamLen);
                concatParam = strcat(strcat(paramSoFar, " "), token);
                parsedMsg.params[counter - 1] = malloc(sizeof(char) * concatParamLen);
                strcpy(parsedMsg.params[counter - 1], concatParam);
                // printf("concat: %s\n", parsedMsg.params[counter - 1]);
            }
        }

        if (!paramIsRest) {
            counter++;
        }
    }

    parsedMsg.numParams = counter;

    /* DEBUG: print out all params parsed */
    // for (int i = 0; i < parsedMsg.numParams; i++)
    // {
    //     printf("%s\n", parsedMsg.params[i]);
    // }

    return parsedMsg;
}

void exec_msg(int clientSocket, char *clientHostname, char *serverHostname, cmd_t msg)
{
    /* Execute parsed message */
    char *replyCode = malloc(sizeof(char) * 3);
    client_info *client = get_client_info(clientHostname);
    if (sameStr(msg.command, "NICK"))
    {
        printf("Command == NICK\n");
        char *nick = msg.params[0];
        if (client == NULL)
        {
            /* NICK == first command */
            printf("NICK is 1st command\n");
            client_info *new_client = malloc(sizeof(client_info));
            new_client->info.nick = malloc(sizeof(char) * strlen(nick));
            strcpy(new_client->info.nick, nick);
            new_client->hostname = malloc(sizeof(char) * strlen(clientHostname));
            strcpy(new_client->hostname, clientHostname);
            add_client(new_client);
        }
        else
        {
            /* NICK == second command */
            printf("NICK is 2nd command\n");
            client->info.nick = malloc(sizeof(char) * strlen(nick));
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
        printf("Command == USER\n");
        char *username = msg.params[0];
        char *realname = msg.params[3];
        if (client != NULL)
        {
            /* USER == second command */
            printf("USER is 2nd command\n");
            client->info.username = malloc(sizeof(char) * strlen(username));
            client->info.realname = malloc(sizeof(char) * strlen(realname));
            strcpy(client->info.username, username);
            strcpy(client->info.realname, realname);
            printf("break1\n");
            if (client->info.nick != NULL)
            {
                printf("break2\n");
                strcpy(replyCode, RPL_WELCOME);
                printf("break3\n");
                send_welcome(
                    clientSocket,
                    replyCode,
                    clientHostname,
                    serverHostname,
                    username,
                    client->info.nick);
                printf("break4\n");
            }
        }
        else
        {
            /* USER == first command */
            printf("USER is 1st command\n");
            client_info *new_client = malloc(sizeof(client_info));
            new_client->info.username = malloc(sizeof(char) * strlen(username));
            strcpy(new_client->info.username, username);
            new_client->info.realname = malloc(sizeof(char) * strlen(realname));
            strcpy(new_client->info.realname, realname);
            new_client->hostname = malloc(sizeof(char) * strlen(clientHostname));
            strcpy(new_client->hostname, clientHostname);
            printf("break5\n");
            add_client(new_client);
            printf("break6\n");
        }
    }

    return;
}