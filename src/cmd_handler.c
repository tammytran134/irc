#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "cmd_handler.h"

bool sameStr(char *s1, char *s2)
{
    /* Check if two strings are the same */
    return strcmp(s1, s2) == 0;
}

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
    else if (sameStr(operator, ">="))
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
    else if (sameStr(operator, "=="))
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
    else
    {
        return false;
    }
}

int handler_NICK(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    if (!(check_cmd(cmd.num_params, NICK_PAM, "==")))
    {
        reply_error(cmd.command, ERR_NONICKNAMEGIVEN, connection);
    }
    else
    {
        client_info_t *clients = ctx->clients_hashtable;
        client_info_t *current_client;
        nick_hb_t *nicks = ctx->nicks_hashtable;
        char *nickname = cmd.params[0];
        client_info_t *client = get_client_w_nick(nickname, &clients, &nicks);
        //if nickname is already in hash table nick
        if (client != NULL && client->hostname != connection->client_hostname)
        {
            /* Nick is already in use and belongs to another client */
            reply_error(nickname, ERR_NICKNAMEINUSE, connection);
            return 0;
        }
        else if (client == NULL)
        {
            /* Client has not entered NICK */
            client = get_client_info(connection->client_hostname, &clients);
            client->info.nick = malloc(sizeof(char) * strlen(nickname));
            strcpy(client->info.nick, nickname);

            /* Add client's nick to server's nicks hash table */
            server_add_nick(ctx, nickname, connection->client_hostname);
            if (!connection->registered)
            {
                /* Update client's connection status */
                connection->registered = true;
                change_connection(ctx, UNKNOWN, DECR);
                change_connection(ctx, KNOWN, INCR);
            }
            if (client->info.username != NULL)
            {
                /* Client has entered USER */
                reply_welcome(client->info, RPL_WELCOME, connection);
            }
        }
        else
        {
            /* Client has entered NICK before (client != NULL) */
            free(client->info.nick);
            /* Update nick in client's entry in clients hash table */
            client->info.nick = malloc(sizeof(char) * strlen(nickname));
            strcpy(client->info.nick, nickname);
            /* Update nick in nicks hashtable */
            server_remove_nick(ctx, nickname);
            server_add_nick(ctx, nickname, client->hostname);
        }
    }

    return 0;
}

int handler_USER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    if (!(check_cmd(cmd.num_params, USER_PAM, "==")))
    {
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection);
    }
    else
    {
        char *client_hostname = connection->client_hostname;
        char *username = cmd.params[0];
        char *realname = cmd.params[3];
        if (has_entered_USER(client_hostname, &ctx->clients_hashtable))
        {
            /* Client has already executed USER command */
            reply_error(username, ERR_ALREADYREGISTRED, connection);
        }
        else
        {
            client_info_t *client = get_client_info(client_hostname,
                                                    &ctx->clients_hashtable);
            client->info.username = malloc(sizeof(char) * strlen(username));
            client->info.realname = malloc(sizeof(char) * strlen(realname));
            if(client->info.nick != NULL)
            {
                reply_welcome(client->info, RPL_WELCOME, connection);
            }
            else
            {
                /* Client has not been registered */
                connection->registered = true;
                change_connection(ctx, UNKNOWN, DECR);
                change_connection(ctx, KNOWN, INCR);
            }
        }
    }
    return 0;
}

int handler_QUIT(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    // send closing link hostname, msg
    // remove user from all hash tables - channels, systems
    // relay back to the channels that the users are in

    /* TODO:
         * if (NICK == NULL || USER == NULL) -> decr_unknown_connectons
         * if (NICK != NULL && USER != NULL) -> 
         */
    return 0;
}

int handler_JOIN(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    if (!(check_cmd(cmd.num_params, JOIN_PAM, "==")))
    {
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection);
    }
    // else
    // if channel exists
    // reply RPL_NAMREPLY and RPL_ENDOFNAMES: list of users on channels
    // relay message to all members of channel and user
    // if doesnt create channel
    // reply RPL_NAMREPLY and RPL_ENDOFNAMES
    // relay message to all members of channel and user
    return 0;
}

int handler_PRIVMSG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{

    // if no name of recipient is identified
    // send ERR_NOSUCHNICK
    // if no text with prefix is sent
    // send ERR_NOTEXTTOSEND
    // if no name of recipient is input
    // send ERR_NORECIPIENT

    // user-to-user
    // identify the recipient and his socket

    // user to channel
    // if send messages to channel they are not in: ERR_CANNOTSENDTOCHAN
    // relay message to all users of channels
    return 0;
}

int handler_NOTICE(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    // identify errors but don't reply
    // send messages if success
    return 0;
}

int handler_LIST(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    // if no params, list all channels
    // if one param: list the channel
    // reply RPL_LIST channel and no of users
    // RPL_LISTEND
    return 0;
}

int handler_MODE(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    // if only channel is provided
    // ERR_NOSUCHCHANNEL

    // ERR_CHANOPRIVSNEEDED you are not operator

    // if mode is unknown
    // ERR_UNKNOWNMODE

    // user not in
    // ERR_USERNOTINCHANNEL
    // if successful
    // relay back to all users
    return 0;
}
int handler_OPER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    if (!(check_cmd(cmd.num_params, OPER_PAM, "==")))
    {
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection);
    }
    // MUST HAVE INFORMATION ON PASSWORD****************
    // if password mismatch
    // ERR_PASSWDMISMATCH

    // if correct
    // RPL_YOUREOPER

    return 0;
}

int handler_PING(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    char reply_msg[MAX_LEN_STR];
    sprintf(reply_msg, "PONG %s\r\n", connection->server_hostname);
    send(connection->client_socket, reply_msg, strlen(reply_msg), 0);
    return 0;
}

int handler_PONG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    // do nothing
    return 0;
}

int handler_LUSERS(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    //RPL_LUSERCLIENT, those who have put in both nick and user
    //RPL_LUSEROP,
    //RPL_LUSERUNKNOWN: haven't received any nick or user **
    //RPL_LUSERCHANNELS,
    //RPL_LUSERME: connections, excluding unknown ones **
    return 0;
}

void exec_cmd(cmd_t full_cmd, connection_info_t *connection, server_ctx_t *ctx)
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
    client_info_t *clients = ctx->clients_hashtable;
    for (i = 0; i < num_handlers; i++)
    {
        if (sameStr(cmd, handlers[i].name))
        {
            if ((has_registered(connection->client_hostname, &clients)) || (sameStr(cmd, "NICK")) || (sameStr(cmd, "USER")))
            {
                handlers[i].func(full_cmd, connection, ctx);
                break;
            }
            else
            {
                reply_error(cmd, ERR_NOTREGISTERED, connection);
            }
        }
    }
    if (i == num_handlers)
    {
        reply_error(cmd, ERR_UNKNOWNCOMMAND, connection);
    }
}