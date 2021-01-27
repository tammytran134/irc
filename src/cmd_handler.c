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

/* check parameter requirement for commands 
 * input: input are no of params we have from input command,
 * standard are standard no of params
 * operator is for us to do necessary comparison function
 * output: true if meet parameter requirement
 */
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

/* function to handler LUSERS command */
int handler_LUSERS(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *client = get_client_info(connection->client_hostname,
                                            &ctx->clients_hashtable);
    //execute RPL_LUSERCLIENT
    int num_of_clients = ctx->num_connections;
    int num_of_services = NUM_SERVICES;
    int num_of_servers = NUM_SERVERS;
    char luserclient[MAX_LEN_STR];
    sprintf(luserclient, ":There are %d users and %d services on %d servers",
            num_of_clients, num_of_services, num_of_servers);
    server_reply(luserclient, RPL_LUSERCLIENT, connection, client);
    //execute RPL_LUSEROP
    int num_of_operators = ctx->irc_operators_hashtable->num_oper;
    char luserop[MAX_LEN_STR];
    sprintf(luserop, "%d :operator(s) online",
            num_of_operators);
    server_reply(luserop, RPL_LUSEROP, connection, client);
    //execute RPL_LUSERUNKNOWN: haven't received any nick or user
    int num_of_unknown_connections = ctx->num_unknown_connections;
    char luserunknown[MAX_LEN_STR];
    sprintf(luserunknown, "%d :unknown connection(s)",
            num_of_unknown_connections);
    server_reply(luserunknown, RPL_LUSERUNKNOWN, connection, client);
    //execute RPL_LUSERCHANNELS
    int num_of_channels = count_channels(&ctx->channels_hashtable);
    char luserchannels[MAX_LEN_STR];
    sprintf(luserchannels, "%d :channels formed",
            num_of_channels);
    server_reply(luserchannels, RPL_LUSERCHANNELS, connection, client);
    //execute RPL_LUSERME: connections, excluding unknown ones
    int num_of_connections = ctx->num_connections;
    char luserme[MAX_LEN_STR];
    sprintf(luserme, ":I have %d clients and %d servers",
            num_of_connections, NUM_SERVERS);
    server_reply(luserme, RPL_LUSERME, connection, client);
    return 0;
}

/* function to handler NICK command */
int handler_NICK(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *clients = ctx->clients_hashtable;
    client_info_t *curr_client = get_client_info(connection->client_hostname,
                                                 &clients);
    if (!(check_cmd(cmd.num_params, NICK_PAM, "==")))
    {
        // ERR_NONICKNAMEGIVEN
        reply_error(cmd.command, ERR_NONICKNAMEGIVEN, connection, curr_client);
        return 0;
    }
    else
    {
        nick_hb_t *nicks = ctx->nicks_hashtable;
        char *nickname = cmd.params[0];
        client_info_t *client = get_client_w_nick(nickname, &clients, &nicks);
        if (client != NULL)
        {
            /* Nick is already in use no matter who the client is */
            // ERR_NICKNAMEINUSE
            reply_error(nickname, ERR_NICKNAMEINUSE, connection, curr_client);
            return 0;
        }
        else
        {
            if (curr_client != NULL && curr_client->info.nick != NULL)
            {
                /* Client has entered NICK before */
                /* Update nick in client's entry in clients hash table */
                curr_client->info.nick = malloc(sizeof(char) * strlen(nickname));
                strcpy(curr_client->info.nick, nickname);
                /* Update nick in nicks hashtable */
                server_remove_nick(ctx, nickname);
                server_add_nick(ctx, nickname, curr_client->hostname);
            }
            else
            {
                /* Client has not entered NICK */
                curr_client->info.nick = malloc(sizeof(char) * strlen(nickname));
                strcpy(curr_client->info.nick, nickname);
                /* Add client's nick to server's nicks hash table */
                server_add_nick(ctx, nickname, connection->client_hostname);
                if (curr_client->info.username != NULL)
                {
                    /* If client has entered USER */
                    reply_welcome(curr_client->info, connection, curr_client);
                    handler_LUSERS(cmd, connection, ctx);
                    reply_error(cmd.command, ERR_NOMOTD, connection, curr_client);
                    return 0;
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

/* function to handler USER command */
int handler_USER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    char *client_hostname = connection->client_hostname;
    client_info_t *client = get_client_info(client_hostname,
                                            &ctx->clients_hashtable);
    if (!(check_cmd(cmd.num_params, USER_PAM, "==")))
    {
        //ERR_NEEDMOREPARAMS
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
            strcpy(client->info.username, username);
            strcpy(client->info.realname, realname);
            if (client->info.nick != NULL)
            {
                /* Client has entered NICK */
                reply_welcome(client->info, connection, client);
                handler_LUSERS(cmd, connection, ctx);
                reply_error(cmd.command, ERR_NOMOTD, connection, client);
    
            }
            else
            {
                /* Client has not entered NICK */
                change_connection(ctx, UNKNOWN, DECR);
                change_connection(ctx, KNOWN, INCR);
            }
        }
    }
    return 0;
}

/* function to handler QUIT command */
int handler_QUIT(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    /* QUIT command handler:
     * Update number of known/unknown connections
     * Remove client from server object context's hash tables
     * Send messages to client and channels acorrdingly
     */
    char *hostname = connection->client_hostname;
    int client_socket = connection->client_socket;
    client_info_t **clients = &ctx->clients_hashtable;
    if (!has_entered_NICK(hostname, clients) &&
        !has_entered_USER(hostname, clients))
    {
        /* Client is classified as unknown connection */
        server_remove_client(ctx, hostname);
        /* -1 number of unknown connections */
        change_connection(ctx, UNKNOWN, DECR);
    }
    else
    {
        /* Client is classified as known connection */
        char *msg;
        if (cmd.num_params == 0)
        {
            msg = "Client Quit";
        }
        else 
        {
            msg = cmd.params[0];
        }
        nick_hb_t **nicks = &ctx->nicks_hashtable;
        channel_hb_t **channels = &ctx->channels_hashtable;
        client_info_t *client = get_client_info(hostname, clients);
        char server_msg[MAX_LEN_STR];
        sprintf(server_msg, ":%s!%s@%s QUIT :%s", client->info.nick,
                client->info.username, connection->client_hostname, msg);
        /* send QUIT message to client */
        char reply_msg[MAX_LEN_STR];
        sprintf(reply_msg, "ERROR :Closing Link: %s (%s)\r\n", 
                            connection->server_hostname, msg);
        send_final(client, reply_msg);
        /* Remove from nicks hash table */
        server_remove_nick(ctx, client->info.nick);
        /* Remove from clients hash table */
        server_remove_client(ctx, hostname);
        /* Iterate through channels that client is in 
         * to remove the clients and to relay client
         * quit message to channel members */
        channel_hb_t *chan;
        for (chan = *channels; chan != NULL; chan = chan->hh.next)
        {
            if (contains_client(hostname, &chan->channel_clients))
            {
                server_remove_chan_client(chan, hostname);
                server_send_chan_client(chan->channel_clients, server_msg, ctx);
            }
        }
        /* -1 number of known connections */
        change_connection(ctx, KNOWN, DECR);

    }
    server_close_socket(ctx, client_socket);
    pthread_exit(NULL);
    return 0;
}

/* function to handler JOIN command */
int handler_JOIN(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    /* JOIN COMMAND: 
     * Parse single parameter channel name from cmd
     * Create channel if channel with given channel name does not exist
     * Add client as member of channel
     * Send messages accordingly
     */
    client_info_t *curr_client = get_client_info(connection->client_hostname,
                                                 &ctx->clients_hashtable);
    if (cmd.num_params < JOIN_PAM)
    {
        //ERR_NEEDMOREPARAMS
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection, curr_client);
        return 0;
    }
    else
    {
        char *channel_name = cmd.params[0];
        channel_hb_t *channel = get_channel_info(channel_name, &ctx->channels_hashtable);
        if (channel == NULL)
        {
            /* Create channel with first user */
            server_add_channel(ctx, channel_name);
            channel = get_channel_info(channel_name, &ctx->channels_hashtable);
        }
        if (contains_client(connection->client_hostname, 
                                    &channel->channel_clients)) 
        {
            return 0;
        }
        /* Add client to channel */
        server_add_chan_client(channel, connection->client_hostname,
                               channel == NULL);
        printf("B4\n");
        /* Channel data after operation */
        channel = get_channel_info(channel_name, &ctx->channels_hashtable);
        channel_client_t *chan_clients = channel->channel_clients;
        printf("B5\n");
        /* Send notification to other members of channel */
        char msg[MAX_LEN_STR];
        sprintf(msg, ":%s!%s@%s JOIN %s\r\n",
                curr_client->info.nick,
                curr_client->info.username,
                curr_client->hostname,
                channel_name);
        server_send_chan_client(chan_clients, msg, ctx);

        /* Send RPL_NAMREPLY AND RPL_ENDOFNAMES to client */
        char reply_msg[MAX_LEN_STR];
        char single_msg[MAX_LEN_STR];
        client_info_t *client;
        channel_client_t *chan_client;

        int i = 0;
        char *hostname;
        sprintf(single_msg, "= %s ", channel_name);
        strcat(reply_msg, single_msg);
        for (chan_client = chan_clients; chan_client != NULL;
             chan_client = chan_client->hh.next)
        {
            hostname = chan_client->hostname;
            client = get_client_info(hostname, &ctx->clients_hashtable);
            if (i == 0)
            {
                sprintf(single_msg, ":@%s", client->info.nick);
            }
            else
            {
                sprintf(single_msg, " %s", client->info.nick);
            }
            strcat(reply_msg, single_msg);
        }
        server_reply(reply_msg, RPL_NAMREPLY, connection, curr_client);
        sprintf(reply_msg, "%s :End of NAMES list", channel_name);
        server_reply(reply_msg, RPL_ENDOFNAMES, connection, curr_client);
    }
    return 0;
}

/* function to handler PRIVMSG command */
int handler_PRIVMSG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *client = get_client_info(connection->client_hostname,
                                            &ctx->clients_hashtable);
    if (cmd.params[0] == NULL)
    {
        // ERR_NORECIPIENT
        reply_error(cmd.command, ERR_NORECIPIENT, connection, client);
        return 0;
    }
    if (cmd.params[1] == NULL)
    {
        // ERR_NOTEXTTOSEND
        reply_error(cmd.command, ERR_NOTEXTTOSEND, connection, client);
        return 0;
    }
    else
    {
        client_info_t *clients = ctx->clients_hashtable;
        nick_hb_t *nicks = ctx->nicks_hashtable;
        channel_hb_t *channels = ctx->channels_hashtable;
        char *receiver_nick = cmd.params[0];
        if (receiver_nick[0] == '#')
        {
            // message to channel
            channel_hb_t *channel = get_channel_info(receiver_nick, &channels);
            if (channel == NULL)
            {
                // ERR_NOSUCHNICK: no channel found
                reply_error(receiver_nick, ERR_NOSUCHNICK, connection, client);
                return 0;
            }
            else
            {   
                // if channel is found and client is in it
                if (contains_client(connection->client_hostname, 
                                    &channel->channel_clients))
                {
                    // relay message to all members of channel
                    char relay_msg[MAX_STR_LEN];
                    sprintf(relay_msg, ":%s!%s@%s QUIT :%s", 
                            client->info.nick,
                            client->info.username, 
                            connection->client_hostname, cmd.params[1]);
                    server_send_chan_client(channel->channel_clients, 
                                            relay_msg, ctx);
                    return 0;
                }
                else
                {
                    // if client is not in channel
                    reply_error(receiver_nick, ERR_CANNOTSENDTOCHAN,
                                connection, client);
                    return 0;
                }
            }
        }
        else
        {
            // message to individual
            client_info_t *receiver = get_client_w_nick(receiver_nick, 
                                                        &clients, &nicks);
            if (receiver == NULL)
            {
                // ERR_NOSUCHNICK no receiver found
                reply_error(receiver_nick, ERR_NOSUCHNICK, connection, client);
                return 0;
            }
            else
            {
                // send message to target client
                relay_reply(cmd.params[1], connection, client, receiver);
                return 0;
            }
        }
    }
    return 0;
}

/* function to handler NOTICE command */
int handler_NOTICE(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    /* logic is the same as PRIVMSG, only that in case
     * of errors, we return and not send anything
     */
    client_info_t *client = get_client_info(connection->client_hostname,
                                            &ctx->clients_hashtable);
    if (cmd.params[0] == NULL)
    {
        return 0;
    }
    if (cmd.params[1] == NULL)
    {
        return 0;
    }
    else
    {
        client_info_t *clients = ctx->clients_hashtable;
        nick_hb_t *nicks = ctx->nicks_hashtable;
        channel_hb_t *channels = ctx->channels_hashtable;
        char *receiver_nick = cmd.params[0];
        if (receiver_nick[0] == '#')
        {
            // message to channel
            channel_hb_t *channel = get_channel_info(receiver_nick, &channels);
            if (channel == NULL)
            {
                return 0;
            }
            else
            {
                if (contains_client(connection->client_hostname, 
                                    &channel->channel_clients))
                {
                    char relay_msg[MAX_STR_LEN];
                    sprintf(relay_msg, ":%s!%s@%s QUIT :%s", client->info.nick,
                                                client->info.username, 
                                                connection->client_hostname, 
                                                cmd.params[1]);
                    server_send_chan_client(channel->channel_clients, 
                                            relay_msg, ctx);
                    return 0;
                }
                else
                {
                    return 0;
                }
            }
        }
        else
        {
            // message to individual
            client_info_t *receiver = get_client_w_nick(receiver_nick, 
                                                    &clients, &nicks);
            if (receiver == NULL)
            {
                return 0;
            }
            else
            {
                relay_reply(cmd.params[1], connection, client, receiver);
                return 0;
            }
        }
    }
    return 0;
}

/* function to handler PART command */
int handler_PART(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *client = get_client_info(connection->client_hostname,
                                            &ctx->clients_hashtable);
    if (!(check_cmd(cmd.num_params, PART_PAM, ">=")))
    {
        // ERR_NEEDMOREPARAMS
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection, client);
        return 0;
    }
    else
    {
        char *channel_name = cmd.params[0];
        channel_hb_t *channel = get_channel_info(channel_name,
                                                &ctx->channels_hashtable);
        if (channel == NULL)
        {
            //ERR_NOSUCHCHANNEL : no channel found
            reply_error(channel_name, ERR_NOSUCHCHANNEL, connection, client);
            return 0;
        }
        if (!(contains_client(connection->client_hostname, 
                                                &channel->channel_clients)))
        {
            // Client is not in channel
            reply_error(channel_name, ERR_NOTONCHANNEL, connection, client);
            return 0;
        }
        else 
        {
            char relay_msg[MAX_STR_LEN];
            char *msg;
            if (cmd.params[1] == NULL)
            {
                msg = client->info.nick;
            }
            else
            {
                msg = cmd.params[1];
            }
            sprintf(relay_msg, "PART %s :%s", channel_name, msg);
            // relay PART message to all members of the channel
            server_send_chan_client(channel->channel_clients, relay_msg, ctx);
            // remove client from the channel
            server_remove_chan_client(channel, connection->client_hostname);
            // if after client has been removed, there are no members left
            // then delete the channel
            if (count_channel_clients(&channel->channel_clients) == 0)
            {
                server_remove_channel(ctx, channel_name);
            }
        }
    }
    return 0;
}

/* function to handler LIST command */
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
    // List all channels
    if (cmd.num_params == 0)
    {
        for (channel = channels; channel != NULL; channel = channels->hh.next)
        {
            channel_name = channel->channel_name;
            num_channel_clients = count_channel_clients
                                                (&channel->channel_clients);
            sprintf(single_msg, "%s # %d:\r\n", channel_name, 
                                                num_channel_clients);
            strcat(reply_msg, single_msg);
        }
        server_reply(reply_msg, RPL_LIST, connection, client);
        server_reply(":End of LIST", RPL_LISTEND, connection, client);
    }
    else
    {
        // list one channel
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

/* function to handler MODE command */
int handler_MODE(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    char *chan_name = cmd.params[0];
    char *mode = cmd.params[1];
    char *nick = cmd.params[2];
    char *hostname = connection->client_hostname;

    client_info_t *curr_client = get_client_info(connection->client_hostname,
                                                 &ctx->clients_hashtable);

    channel_hb_t *channel = get_channel_info(chan_name,
                                             &ctx->channels_hashtable);
    if (channel == NULL)
    {
        /* ERR_NOSUCHCHANNEL */
        reply_error(chan_name, ERR_NOSUCHCHANNEL, connection, curr_client);
        return 0;
    }

    if (!sameStr(mode, "+o") && !sameStr(mode, "-o"))
    {
        /* UNKNOWNMODE */
        reply_error_mult(mode, chan_name, ERR_UNKNOWNMODE,
                         connection, curr_client);
        return 0;
    }

    channel_client_t *chan_client = get_channel_client(hostname,
                                                    &channel->channel_clients);
    if (!sameStr(chan_client->mode, "+o") && 
                                        (!(curr_client->info.is_irc_operator)))
    {
        /* ERR_CHANOPRIVSNEEDED */
        reply_error(chan_name, ERR_CHANOPRIVSNEEDED, connection, curr_client);
        return 0;
    }

    client_info_t *target_client = get_client_w_nick(
        nick,
        &ctx->clients_hashtable,
        &ctx->nicks_hashtable);
    if (!contains_client(target_client->hostname, &channel->channel_clients))
    {

        /* ERR_USERNOTINCHANNEL */
        reply_error_mult(nick, chan_name, ERR_USERNOTINCHANNEL,
                         connection, curr_client);
        return 0;
    }

    if (strlen(mode) == 2)
    {
        /* Update target nick's mode and notify channel */
        strcpy(chan_client->mode, mode);
        char relay_msg[MAX_STR_LEN];
        sprintf(relay_msg, ":%s!%s@%s MODE %s %s %s",
                curr_client->info.nick,
                curr_client->info.username,
                curr_client->hostname,
                chan_name,
                mode,
                nick);
        server_send_chan_client(channel->channel_clients, relay_msg, ctx);
    }

    return 0;
}

/* function to handler OPER command */
int handler_OPER(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *client = get_client_info(connection->client_hostname,
                                            &ctx->clients_hashtable);
    if (!(check_cmd(cmd.num_params, OPER_PAM, "==")))
    {
        // ERR_NEEDMOREPARAMS
        reply_error(cmd.command, ERR_NEEDMOREPARAMS, connection, client);
        return 0;
    }
    if (!(sameStr(cmd.params[1], ctx->password)))
    {
        // If password doesn't match -> ERR_PASSWDMISMATCH
        reply_error(cmd.command, ERR_PASSWDMISMATCH, connection, client);
        return 0;
    }
    else
    {
        // add client to hash table of irc_operator
        server_add_irc_operator(ctx, cmd.params[0], OPERATOR_MODE);
        client->info.is_irc_operator = true;
        // send RPL_YOUREOPER to client
        server_reply(":You are now an IRC operator", RPL_YOUREOPER, 
                                                        connection, 
                                                        client);
    }

    return 0;
}

/* function to handler PING command */
int handler_PING(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    client_info_t *client = get_client_info(connection->client_hostname,
                                            &ctx->clients_hashtable);
    // send back PONG message
    char reply_msg[MAX_LEN_STR];
    sprintf(reply_msg, "PONG %s\r\n", connection->server_hostname);
    send_final(client, reply_msg);
    return 0;
}

/* function to handler PONG command */
int handler_PONG(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    // do nothing
    return 0;
}

/* function to handler WHOIS command */
int handler_WHOIS(cmd_t cmd, connection_info_t *connection, server_ctx_t *ctx) 
{
    char *client_hostname = connection->client_hostname;
    client_info_t *clients = ctx->clients_hashtable;
    client_info_t *client = get_client_info(client_hostname, &clients);
    if (!(check_cmd(cmd.num_params, WHOIS_PAM, ">=")))
    {
        return 0;
    }
    else {
        nick_hb_t *nicks = ctx->nicks_hashtable;
        char *nickname = cmd.params[0];
        client_info_t *target_client = get_client_w_nick(nickname, 
                                                        &clients, &nicks);
        if (target_client == NULL)
        {
            reply_error(nickname, ERR_NOSUCHNICK, connection, client);
            return 0;
        }
        else {

            /* RPL_WHOISUSER */
            char whoisuser[MAX_LEN_STR];
            sprintf(whoisuser, "%s %s %s * :%s", 
                            target_client->info.nick, 
                            target_client->info.username, 
                            target_client->server_hostname, 
                            target_client->info.realname);
            server_reply(whoisuser, RPL_WHOISUSER, connection, client);
            /* RPL_WHOISCHANNELS */
            // bool channel_member = false;
            // channel_hb_t **channels = &ctx->channels_hashtable;
            // channel_hb_t *chan;
            // char whoischannels[MAX_LEN_STR];
            // for (chan = *channels; chan != NULL; chan = chan->hh.next)
            // {
            //     if (contains_client(target_client->hostname, &chan->channel_clients))
            //     {
            //         channel_member = true;
            //         break;
            //     }
            // }
            // char whoischannels[MAX_LEN_STR];
            // sprintf(whoischannels, "%s %s :Project1b", 
            //         target_client->info.nick, target_client->server_hostname);
            // server_reply(whoischannels, RPL_WHOISCHANNELS, connection, client);

            /* RPL_WHOISSERVER */
            char whoisserver[MAX_LEN_STR];
            sprintf(whoisserver, "%s %s :Project1b", 
                    target_client->info.nick, target_client->server_hostname);
            server_reply(whoisserver, RPL_WHOISSERVER, connection, client);
            /* RPL_AWAY */
            /* RPL_WHOISOPERATOR */
            if (client->info.is_irc_operator)
            {
                if (target_client->info.is_irc_operator)
                {
                    char whoisoperator[MAX_LEN_STR];
                    sprintf(whoisoperator, "%s :is an IRC operator", 
                    target_client->info.nick);
                    server_reply(whoisoperator, RPL_WHOISOPERATOR, 
                                    connection, client);
                }
            }
            /* RPL_ENDOFWHOIS */
            char endofwhois[MAX_LEN_STR];
            sprintf(endofwhois, "%s :End of WHOIS list", nickname);
            server_reply(endofwhois, RPL_ENDOFWHOIS, connection, client);
            return 0;
        }
    }
}

void print_cmd(cmd_t full_cmd)
{
    /* DEBUG tool */
    printf("command: %s\n", full_cmd.command);
    printf("num of params: %d\n", full_cmd.num_params);
    for(int i = 0; i < full_cmd.num_params; i++)
    {
        printf("param[%d]: %s\n", i, full_cmd.params[i]);
    }
    return;
}

void exec_cmd(cmd_t full_cmd, connection_info_t *connection, server_ctx_t *ctx)
{
    // dispatch table that maps first string in command to a handler function
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
        {"PART", handler_PART},
        {"WHOIS", handler_WHOIS}
    };
    int num_handlers = sizeof(handlers) / sizeof(handler_entry_t);
    char *cmd = full_cmd.command;
    int i;
    printf("BEFORE 1?\n");
    client_info_t *client = get_client_info(connection->client_hostname,
                                            &ctx->clients_hashtable);
    // if client has registered
    bool registered = ((client->info.nick != NULL) 
                                        && (client->info.username != NULL));
    for (i = 0; i < num_handlers; i++)
    {
        // if command is found in dispatch table
        if (sameStr(cmd, handlers[i].name))
        {
            // if client is registered, then process command
            // else if command is NICK or USER then process command
            if ((registered) || (sameStr(cmd, "NICK")) || 
                                                        (sameStr(cmd, "USER")))
            {
                printf("ERROR 2a\n");
                print_cmd(full_cmd);
                handlers[i].func(full_cmd, connection, ctx);
                printf("ERROR 2b\n");
                break;
            }
            else
            {
                // If client hasn't completed registration,
                // return ERR_NOTREGISTERED
                reply_error(cmd, ERR_NOTREGISTERED, connection, client);
                printf("ERROR 3a\n");
                break;
            }
        }
    }
    // If command is not found in dispatch table, 
    // and client has registered, return ERR_UNKNOWNCOMMAND
    if ((i == num_handlers) && (registered))
    {
        reply_error(cmd, ERR_UNKNOWNCOMMAND, connection, client);
    }
}