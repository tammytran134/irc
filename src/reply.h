/*
 *  chirc: a simple multi-threaded IRC server
 *
 *  Reply codes
 *
 */
/*
 *  Copyright (c) 2011-2020, The University of Chicago
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or withsend
 *  modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  - Neither the name of The University of Chicago nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software withsend specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY send OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef REPLY_H_
#define REPLY_H_

#include "server_info.h"

#define MAX_LEN_STR 1024

#define RPL_WELCOME "001"
#define RPL_YOURHOST "002"
#define RPL_CREATED "003"
#define RPL_MYINFO "004"

#define RPL_LUSERCLIENT "251"
#define RPL_LUSEROP "252"
#define RPL_LUSERUNKNOWN "253"
#define RPL_LUSERCHANNELS "254"
#define RPL_LUSERME "255"

#define RPL_AWAY "301"
#define RPL_UNAWAY "305"
#define RPL_NOWAWAY "306"

#define RPL_WHOISUSER "311"
#define RPL_WHOISSERVER "312"
#define RPL_WHOISOPERATOR "313"
#define RPL_WHOISIDLE "317"
#define RPL_ENDOFWHOIS "318"
#define RPL_WHOISCHANNELS "319"

#define RPL_WHOREPLY "352"
#define RPL_ENDOFWHO "315"

#define RPL_LIST "322"
#define RPL_LISTEND "323"

#define RPL_CHANNELMODEIS "324"

#define RPL_NOTOPIC "331"
#define RPL_TOPIC "332"

#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"

#define RPL_MOTDSTART "375"
#define RPL_MOTD "372"
#define RPL_ENDOFMOTD "376"

#define RPL_YOUREOPER "381"

#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHSERVER "402"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CANNOTSENDTOCHAN "404"
#define ERR_NORECIPIENT "411"
#define ERR_NOTEXTTOSEND "412"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NOMOTD "422"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_NICKNAMEINUSE "433"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL "442"
#define ERR_NOTREGISTERED "451"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_UNKNOWNMODE "472"
#define ERR_NOPRIVILEGES "481"
#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_UMODEUNKNOWNFLAG "501"
#define ERR_USERSDONTMATCH "502"

#define VERSION "1"



/* These 2 functions below specifically deal with constructing
 * error messages to send to client
 */

/* This function takes in the string that needs to be
 * passed into the error reply, the reply code, and
 * connection_info_t and client_info_t that have
 * client's and server's information to construct
 * error reply messages
 * Output: void, just send messages
 */
void reply_error(char *cmd, char *reply_code, connection_info_t *connection, 
                client_info_t *client);

/* This function takes in 2 strings that need to be
 * passed into the reply, the reply code, and
 * connection_info_t and client_info_t that have
 * client's and server's information to construct
 * error reply messages
 * Output: void, just send messages
 */
void reply_error_mult(char *cmd1, char *cmd2, char *reply_code, 
                connection_info_t *connection, client_info_t *client);


/* Server_reply function deals with sending messages
 * to the client, it takes in the messages that 
 * we want to send, the reply code, and
 * connection_info_t and client_info_t that has
 * client's and server's information and
 * concatenate the appropriate prefix to the messages
 * Output: void, just send messages
 */
void server_reply(char *msg, char *reply_code, connection_info_t *connection, 
                client_info_t *client);

/* relay_reply function deals with sending *relay* messages
 * from the client to other clients and channels.
 * It takes in the messages that 
 * we want to send, the reply code, and
 * connection_info_t and client_info_t that have
 * client's and server's information and
 * concatenate the appropriate prefix to the messages
 * Output: void, just send messages
 */
void relay_reply(char *msg, connection_info_t *connection, 
                client_info_t *sender, client_info_t *receiver);

/* Send all replies required after a user has finished
 * registering: RPL_WELCOME, RPL_YOURHOST, Send RPL_CREATED, 
 * Send RPL_MYINFO
 * Input: client_info_t and connection_info_t struct 
 * for information on user's server and registration information
 * Output: void, just send messages
 */
void reply_welcome(connection_info_t *connection, 
                    client_info_t *client);

#endif /* REPLY_H_ */
