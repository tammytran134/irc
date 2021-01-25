#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "msg_handler.h"
#include "reply.h"

bool sameStr(char *s1, char *s2)
{
    /* Check if two strings are the same */
    return strcmp(s1, s2) == 0;
}

void send_welcome(
    int client_socket,
    char *reply_code,
    char *client_hostname,
    char *server_hostname,
    char *username,
    char *nick)
{
    /* Format and send welcome message to client */
    char reply_msg[1024];
    sprintf(reply_msg,
            ":%s %s %s :Welcome to the Internet Relay Network %s!%s@%s\r\n",
            server_hostname,
            reply_code,
            nick,
            nick,
            username,
            client_hostname);

    send(client_socket, reply_msg, strlen(reply_msg), 0);
    return;
}

msg_t recv_msg(
    char *buf,
    msg_t rmsg,
    client_info_t **clients,
    int client_socket,
    char *client_hostname,
    char *server_hostname)
{
    /* Receives and process incoming message from server:
     * buf: buffer for incoming mssage
     * rmsg: holds data for incoming message until a complete message is formed
     * clients: pointer to clients hashtable's pointer
     * This message returns the updated msg_t struct back to the main function,
     * so that the msg_t state gets updated between each recv() call
     */
    char c;
    for (int i = 0; i < strlen(buf); i++)
    {
        c = buf[i];
        /* If the command being sent contains more than 512 characters 
         * then we truncate the message and process it anyways
         */
        if (rmsg.counter == MAX_MSG_LEN)
        {
            /* copy the message to a new char array to process it */
            char copy_msg[MAX_MSG_LEN];
            strcpy(copy_msg, rmsg.msg);
            /* check if the command being sent is NICK or USER
             * if yes, then turn boolean field NICK or USER to 
             * true to pass it back to main function
             */
            cmd_t cmd = parse_msg(copy_msg);
            if (rmsg.nick_cmd == false)
            {
                if (sameStr(cmd.command, "NICK"))
                {
                    rmsg.nick_cmd = true;
                }
            }
            if (rmsg.user_cmd == false)
            {
                if (sameStr(cmd.command, "USER"))
                {
                    rmsg.user_cmd = true;
                }
            }
            /* process it */
            exec_msg(client_socket, clients, client_hostname, 
                    server_hostname, cmd);
            /* renew the msg_t struct to wipe out the char *msg buffer
             * and renew the counter to hold new message
             * after current command has been sent away to be processed
             */
            char *new_msg = (char *)malloc(sizeof(char) * MAX_MSG_LEN);
            rmsg.msg = new_msg;
            rmsg.counter = 0;
            return rmsg;
        }
        /* If not message overflow, then add character to msg buffer */
        else
        {
            rmsg.msg[rmsg.counter] = c;
            /* If end of command */
            if (c == '\n')
            {
                if (rmsg.msg[rmsg.counter - 1] == '\r')
                {
                    /* copy the message to a new char array to process it */
                    char copy_msg[strlen(rmsg.msg)];
                    strcpy(copy_msg, rmsg.msg);
                    /* check if the command being sent is NICK or USER
                     * if yes, then turn boolean field NICK or USER to 
                     * true to pass it back to main function
                     */
                    cmd_t cmd = parse_msg(copy_msg);
                    if (rmsg.nick_cmd == false)
                    {
                        if (sameStr(cmd.command, "NICK"))
                        {
                            rmsg.nick_cmd = true;
                        }
                    }
                    if (rmsg.user_cmd == false)
                    {
                        if (sameStr(cmd.command, "USER"))
                        {
                            rmsg.user_cmd = true;
                        }
                    }
                    exec_msg(client_socket, clients, 
                            client_hostname, server_hostname, 
                            cmd);
                    /* renew the msg_t struct to wipe out the char *msg buffer
                     * and renew the counter to hold new message
                     * after current command has been sent away to be processed
                     */
                    char *new_msg = (char *)malloc(sizeof(char) * MAX_MSG_LEN);
                    rmsg.msg = new_msg;
                    rmsg.counter = 0;
                }
                else
                {
                    rmsg.counter++;
                }
            }
            else
            {
                rmsg.counter++;
            }
        }
    }
    return rmsg;
}

cmd_t parse_msg(char *msg_buffer)
{
    /* Parse command from message buffer into command struct */
    char *token;
    char *rest = msg_buffer;

    cmd_t parsed_msg;
    parsed_msg.num_params = 0;
    int counter = 0;

    /* Flag to indicate a param that takes up the rest of the message */
    bool param_is_rest = false;
    while ((token = strtok_r(rest, " \t\r\n", &rest)))
    {
        if (counter == 0)
        {
            /* token == command keyword */
            parsed_msg.command = malloc(sizeof(char) * strlen(token));
            strcpy(parsed_msg.command, token);
        }
        else if (counter < MAX_PARAMS)
        {
            /* Token is a param */
            if (!param_is_rest)
            {
                /* Param does not take up the rest of the message */ 
                size_t token_size = sizeof(char) * strlen(token);
                parsed_msg.params[counter - 1] = malloc(token_size);
                if (token[0] == ':')
                    /* Param takes up the rest of the messsage */
                    param_is_rest = true;
                token = strtok(token, ":");
                strcpy(parsed_msg.params[counter - 1], token);
            }
            else
            {
                /* Param takes up rest of message */
                /* Accumulate the rest of param into params[counter - 1] */
                char *param_so_far = parsed_msg.params[counter - 1];
                int concat_param_len = strlen(param_so_far) 
                                        + strlen(token) + 1;
                char *concat_param = malloc(sizeof(char) * concat_param_len);
                concat_param = strcat(strcat(param_so_far, " "), token);
                free(parsed_msg.params[counter - 1]);
                size_t new_param_size = sizeof(char) * concat_param_len;
                parsed_msg.params[counter - 1] = malloc(new_param_size);
                strcpy(parsed_msg.params[counter - 1], concat_param);
            }
        }

        if (!param_is_rest)
        {
            /* Counter only increments when param does not take up the rest of
            the message */
            counter++;
        }
    }

    parsed_msg.num_params = counter;

    return parsed_msg;
}

void exec_msg(
    int client_socket,
    client_info_t **clients,
    char *client_hostname,
    char *server_hostname,
    cmd_t msg)
{
    /* Execute parsed message */
    char *reply_code = malloc(sizeof(char) * 3);
    /* Get client data from hashtable. Return NULL if client is not found */
    client_info_t *client = get_client_info(client_hostname, clients);

    if (sameStr(msg.command, "NICK"))
    {
        /* Command == "NICK" */
        char *nick = msg.params[0];
        if (client == NULL)
        {
            /* NICK is the first command from client */
            client_info_t *new_client = malloc(sizeof(client_info_t));
            /* Populate new client's struct */
            new_client->info.nick = malloc(sizeof(char) * strlen(nick));
            strcpy(new_client->info.nick, nick);
            size_t client_hostname_len = strlen(client_hostname);
            new_client->hostname = malloc(sizeof(char) * client_hostname_len);
            strcpy(new_client->hostname, client_hostname);
            new_client->info.username = NULL;
            new_client->info.realname = NULL;
            /* Add new client to clients hashtable */
            add_client(new_client, clients);
        }
        else
        {
            /* NICK is called after USER command */
            /* Update client's nick */
            client->info.nick = malloc(sizeof(char) * strlen(nick));
            strcpy(client->info.nick, nick);
            if (client->info.username != NULL)
            {
                /* Client has already called the USER command */
                strcpy(reply_code, RPL_WELCOME);
                send_welcome(
                    client_socket,
                    reply_code,
                    client_hostname,
                    server_hostname,
                    client->info.username,
                    client->info.nick);
            }
        }
    }
    else if (sameStr(msg.command, "USER"))
    {
        /* Command == "USER" */
        char *username = msg.params[0];
        char *realname = msg.params[3];
        if (client != NULL)
        {
            /* USER is called after NICK command */
            /* Update client's username and real name */
            client->info.username = malloc(sizeof(char) * strlen(username));
            client->info.realname = malloc(sizeof(char) * strlen(realname));
            strcpy(client->info.username, username);
            strcpy(client->info.realname, realname);
            if (client->info.nick != NULL)
            {
                /* Client has already called the NICK command */
                strcpy(reply_code, RPL_WELCOME);
                send_welcome(
                    client_socket,
                    reply_code,
                    client_hostname,
                    server_hostname,
                    username,
                    client->info.nick);
            }
        }
        else
        {
            /* USER is the first command from client */
            client_info_t *new_client = malloc(sizeof(client_info_t));
            /* Populate new client's struct */
            size_t username_size = sizeof(char) * strlen(username);
            new_client->info.username = malloc(username_size);
            strcpy(new_client->info.username, username);
            size_t realname_size = sizeof(char) * strlen(realname);
            new_client->info.realname = malloc(realname_size);
            strcpy(new_client->info.realname, realname);
            size_t hostname_size = sizeof(char) * strlen(client_hostname);
            new_client->hostname = malloc(hostname_size);
            strcpy(new_client->hostname, client_hostname);
            new_client->info.nick = NULL;
            /* Add new client to clients' hashtable */
            add_client(new_client, clients);
        }
    }

    return;
}