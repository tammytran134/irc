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
    client_info_t *clients = ctx->clients_hashtable;
    client_info_t *current_client = get_client_info(connection->client_hostname, &clients);
    if (!(check_cmd(cmd.num_params, NICK_PAM, "==")))
    {
        reply_error(cmd.command, ERR_NONICKNAMEGIVEN, connection, current_client);
        return 0;
    }
    else
    {
        client_info_t *current_client;
        nick_hb_t *nicks = ctx->nicks_hashtable;
        char *nickname = cmd.params[0];
        client_info_t *client = get_client_w_nick(nickname, &clients, &nicks);
        //if nickname is already in hash table nick
        if (client != NULL)
        {
            /* Nick is already in use no matter who the client is */
            reply_error(nickname, ERR_NICKNAMEINUSE, connection, current_client);
            return 0;
        }
        else
        {
            //client = get_client_info(connection->client_hostname, &clients);
            if (current_client->info.nick != NULL)
            {
                /* Client has entered NICK before */
                free(current_client->info.nick);
                /* Update nick in client's entry in clients hash table */
                current_client->info.nick = malloc(sizeof(char) * strlen(nickname));
                strcpy(current_client->info.nick, nickname);
                /* Update nick in nicks hashtable */
                server_remove_nick(ctx, nickname);
                server_add_nick(ctx, nickname, current_client->hostname);
            }
            else
            {
                /* Client has not entered NICK */
                current_client->info.nick = malloc(sizeof(char) * strlen(nickname));
                strcpy(current_client->info.nick, nickname);

                /* Add client's nick to server's nicks hash table */
                server_add_nick(ctx, nickname, connection->client_hostname);
                if (current_client->info.username != NULL)
                {
                    /* If client has entered USER */
                    connection->registered = true;
                    change_connection(ctx, UNKNOWN, DECR);
                    change_connection(ctx, KNOWN, INCR);
                    reply_welcome(current_client->info, connection, current_client);
                }
                else
                {
                    /* Client has not entered USER */
                    change_connection(ctx, UNKNOWN, DECR);
                    change_connection(ctx, KNOWN, INCR);
                }
            }
        }
    }
    return 0;
}

int handler_USER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    char *client_hostname = connection->client_hostname;
    client_info_t *client = get_client_info(client_hostname,
                            &ctx->clients_hashtable);
    if (!(check_cmd(cmd.num_params, USER_PAM, "==")))
    {
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection, client);
        return 0;
    }
    else
    {
        char *client_hostname = connection->client_hostname;
        char *username = cmd.params[0];
        char *realname = cmd.params[3];
        if (has_entered_USER(client_hostname, &ctx->clients_hashtable))
        {
            /* Client has already executed USER command */
            reply_error(username, ERR_ALREADYREGISTRED, connection, client);
            return 0;
        }
        else
        {
            client->info.username = malloc(sizeof(char) * strlen(username));
            client->info.realname = malloc(sizeof(char) * strlen(realname));
            if (client->info.nick != NULL)
            {
                reply_welcome(client->info, connection, client);
                /* Client has not been registered */
                connection->registered = true;
            }
            else
            {
                change_connection(ctx, UNKNOWN, DECR);
                change_connection(ctx, KNOWN, INCR);
            }
        }
    }
    return 0;
}

int handler_QUIT(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    /* QUIT command handler:
     * Update number of known/unknown connections
     * Remove client from server object context's hash tables
     * Send messages to client and channels acorrdingly
     */
    char *hostname = connection->client_hostname;
    client_info_t **clients = &ctx->clients_hashtable;
    if (!has_entered_NICK(hostname, clients) &&
        !has_entered_USER(hostname, clients))
    {
        /* -1 number of unknown connections */
        change_connection(ctx, UNKNOWN, DECR);
    }
    else
    {
        nick_hb_t **nicks = &ctx->nicks_hashtable;
        channel_hb_t **channels = &ctx->channels_hashtable;
        client_info_t *client = get_client_info(hostname, clients);
        /* Remove from nicks hash table */
        if (client != NULL && client->info.nick != NULL)
            server_remove_nick(ctx, client->info.nick);
        /* Remove from clients hash table */
        server_remove_client(ctx, hostname);
        /* Iterate through channels that client is in */
        channel_hb_t *chan;
        for (chan = *channels; chan != NULL; chan = chan->hh.next)
        {
            if (contains_client(hostname, &chan->channel_clients))
            {
                server_remove_chan_client(chan, hostname);
                // TODO: send msg to other members in channel
            }
        }
        /* -1 number of known connections */
        change_connection(ctx, KNOWN, DECR);
        // TODO: send closing link, hostname
    }
    return 0;
}

int handler_JOIN(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *client = get_client_info(connection->client_hostname,
                            &ctx->clients_hashtable);
    if (!(check_cmd(cmd.num_params, JOIN_PAM, "==")))
    {
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection, client);
        return 0;
    }
    // else
    // if channel exists
    // reply RPL_NAMREPLY and RPL_ENDOFNAMES: list of users on channels
    // relay message to all members of channel and user
    // if doesnt create channel
    // reply RPL_NAMREPLY and RPL_ENDOFNAMES
    // relay message to all members of channel and user
    else
    {
        channel_hb_t *channel;
    }
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
    char reply_msg[MAX_LEN_STR];
    char single_msg[MAX_LEN_STR];
    channel_hb_t *channels = ctx->channels_hashtable;
    channel_hb_t *channel = NULL;
    int num_channel_clients;
    char *channel_name;
    client_info_t *client = get_client_info(connection->client_hostname,
                            &ctx->clients_hashtable);
    if (cmd.num_params == 0)
    {
        for (channel=channels; channel != NULL; channel=channels->hh.next) 
        {
            channel_name = channel->channel_name;
            num_channel_clients = count_channel_clients(&channel->channel_clients);
            sprintf(single_msg, "%s # %d:\r\n", channel_name, num_channel_clients);
            strcat(reply_msg, single_msg);
        }
        server_reply(reply_msg, RPL_LIST, connection, client);
        server_reply(":End of LIST", RPL_LISTEND, connection, client);
    }
    else 
    {
        channel_name = cmd.params[0];
        channel_hb_t *channel = get_channel_info(channel_name,
                        &ctx->channels_hashtable);
        num_channel_clients = count_channel_clients(&channel->channel_clients);
        sprintf(single_msg, "%s # %d:\r\n", channel_name, num_channel_clients);
        server_reply(reply_msg, RPL_LIST, connection, client);
        server_reply(":End of LIST", RPL_LISTEND, connection, client);
    }
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
    client_info_t *client = get_client_info(connection->client_hostname,
                            &ctx->clients_hashtable);
    if (!(check_cmd(cmd.num_params, OPER_PAM, "==")))
    {
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection, client);
        return 0;
    }
    if (!(sameStr(cmd.params[1], ctx->password)))
    {
        reply_error(cmd.command, ERR_PASSWDMISMATCH, connection, client);
        return 0;
    }
    else
    {
        // TODO with thread:
        // increment ctx->irc_operators_hashtable->num_oper++;
        // add user's nick to ctx->irc_operators_hashtable->irc_oper;
        // turn is_irc_operator field in client_info_t to true;
        server_reply(":You are now an IRC operator", RPL_YOUREOPER, connection, client);
    }

    return 0;
}

int handler_PING(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *client = get_client_info(connection->client_hostname,
                            &ctx->clients_hashtable);
    char reply_msg[MAX_LEN_STR];
    sprintf(reply_msg, "PONG %s\r\n", connection->server_hostname);
    send_final(client, reply_msg);
    return 0;
}

int handler_PONG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    // do nothing
    return 0;
}

int handler_LUSERS(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *client = get_client_info(connection->client_hostname,
                            &ctx->clients_hashtable);
    //RPL_LUSERCLIENT, those who have put in both nick and user
    int num_of_clients = ctx->num_connections;
    int num_of_services = NUM_SERVICES;
    int num_of_servers = NUM_SERVERS;
    char luserclient[MAX_LEN_STR];
    sprintf(luserclient, ":There are %d users and %d services on %d servers",
            num_of_clients, num_of_services, num_of_servers);
    server_reply(luserclient, RPL_LUSERCLIENT, connection, client);
    //RPL_LUSEROP
    int num_of_operators = ctx->irc_operators_hashtable->num_oper;
    char luserop[MAX_LEN_STR];
    sprintf(luserop, "%d :operator(s) online",
            num_of_operators);
    server_reply(luserop, RPL_LUSEROP, connection, client);
    //RPL_LUSERUNKNOWN: haven't received any nick or user
    int num_of_unknown_connections = ctx->num_unknown_connections;
    char luserunknown[MAX_LEN_STR];
    sprintf(luserunknown, "%d :unknown connection(s)",
            num_of_unknown_connections);
    server_reply(luserunknown, RPL_LUSERUNKNOWN, connection, client);
    //RPL_LUSERCHANNELS
    int num_of_channels = count_channels(&ctx->channels_hashtable);
    char luserchannels[MAX_LEN_STR];
    sprintf(luserchannels, "%d :channels formed",
            num_of_channels);
    server_reply(luserchannels, RPL_LUSERCHANNELS, connection, client);
    //RPL_LUSERME: connections, excluding unknown ones 
    int num_of_connections = ctx->num_connections;
    char luserme[MAX_LEN_STR];
    sprintf(luserme, ":I have %d clients and %d servers",
            num_of_connections, NUM_SERVERS);
    server_reply(luserme, RPL_LUSERME, connection, client);
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
    client_info_t *client = get_client_info(connection->client_hostname,
                            &ctx->clients_hashtable);
    for (i = 0; i < num_handlers; i++)
    {
        if (sameStr(cmd, handlers[i].name))
        {
            if ((connection->registered) ||
                (sameStr(cmd, "NICK")) ||
                (sameStr(cmd, "USER")))
            {
                handlers[i].func(full_cmd, connection, ctx);
                break;
            }
            else
            {
                reply_error(cmd, ERR_NOTREGISTERED, connection, client);
            }
        }
    }
    if (i == num_handlers)
    {
        reply_error(cmd, ERR_UNKNOWNCOMMAND, connection, client);
    }
}