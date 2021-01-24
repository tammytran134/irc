#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "cmd_handler.h"

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
    if (sameStr(operator, ">="))
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
    if (sameStr(operator, "=="))
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
}

int handler_NICK(cmd_t cmd) 
{
    if (!(check_cmd(cmd.num_params, NICK_PAM, "==")))
    {
        //reply(ERR_NONICKNAMEGIVEN);
    }
    else
    {
        // if cmd.params[0] is already in hash table nick
        // throw ERR_NICKNAMEINUSE
        // else 
        //{
            //client_info_t *client = get_client_info(client_hostname, ctx->clients_hashtable);
            // have to check for cases when NICK is first command, second command
            //change the user info 
            // if the user is registered send reply
            // if the user is already there, relay messages to all channels
        //}
    }

    return 0;
}

int handler_USER(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, USER_PAM, "==")))
    {
        // NEEDMOREPARAMS
    }
    else
    {
        //if already register - check the hashtable
        // ERR_ALREADYREGISTRED
        //
        //else
        //fill in the detail
        // if registered send reply
    }
    return 0;
} 

int handler_QUIT(cmd_t cmd)
{
        // send closing link hostname, msg
        // remove user from all hash tables - channels, systems
        // relay back to the channels that the users are in
    return 0;
}

int handler_JOIN(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, JOIN_PAM, "==")))
    {
        // NEEDMOREPARAMS
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

int handler_PRIVMSG(cmd_t cmd)
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

int handler_NOTICE(cmd_t cmd)
{
    // identify errors but don't reply
    // send messages if success
    return 0;
}

int handler_LIST(cmd_t cmd)
{
    // if no params, list all channels
    // if one param: list the channel
    // reply RPL_LIST channel and no of users
    // RPL_LISTEND
    return 0;
}

int handler_MODE(cmd_t cmd)
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
int handler_OPER(cmd_t cmd)
{
    if (!(check_cmd(cmd.num_params, OPER_PAM, "==")))
    {
        // error NEEDMOREPARAMS
    }
    // MUST HAVE INFORMATION ON PASSWORD****************
    // if password mismatch
    // ERR_PASSWDMISMATCH

    // if correct
    // RPL_YOUREOPER

    return 0;
}

int handler_PING(cmd_t cmd)
{
    // send pong message to client
    return 0;
}

int handler_PONG(cmd_t cmd)
{
    // do nothing
    return 0;
}

int handler_LUSERS(cmd_t cmd)
{
    //RPL_LUSERCLIENT, those who have put in both nick and user
    //RPL_LUSEROP, 
    //RPL_LUSERUNKNOWN: haven't received any nick or user **
    //RPL_LUSERCHANNELS, 
    //RPL_LUSERME: connections, excluding unknown ones **
    return 0;
}

void exec_cmd(cmd_t full_cmd)
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
    for (i = 0; i < num_handlers; i++) 
    {
        // check for ERR_UNREGISTERED error NEED A VARIABLE TO CHECK IF A USER IS REGISTERED********
        if (sameStr(cmd, handlers[i].name)) {
            handlers[i].func(full_cmd);
            break;
        }
    }
    if (i == num_handlers)
    {
        // send ERR_UNKNOWNCOMMAND
    }
}