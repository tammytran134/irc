#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "msg_handler.h"

msg_t recv_msg(
    char *buf,
    msg_t rmsg,
    server_ctx_t *ctx,
    connection_info_t *connection)
{
    /* Receives and process incoming message from server:
     * buf: buffer for incoming mssage
     * rmsg: holds data for incoming message until a complete message is formed
     * clients: pointer to clients hashtable's pointer
     * This message returns the updated msg_t struct back to the main function,
     * so that the msg_t state gets updated between each recv() call
     */
    printf ("it comes to recv_msg\n");
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
            //exec_msg(ctx, cmd, connection);
            exec_cmd(cmd, connection, ctx);
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
                    //exec_msg(ctx, cmd, connection);
                    exec_cmd(cmd, connection, ctx);
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
    printf("msg: %s", msg_buffer);

    cmd_t parsed_msg;
    parsed_msg.num_params = 0;
    int counter = 0;

    /* Flag to indicate a param that takes up the rest of the message */
    bool param_is_rest = false;
    while ((token = strtok_r(rest, " \t\r\n", &rest)) != NULL)
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
                // size_t token_size = sizeof(char) * strlen(token);
                parsed_msg.params[counter - 1] = malloc(sizeof(char) * MAX_STR_LEN);
                if (token[0] == ':')
                {
                    /* Param takes up the rest of the messsage */
                    param_is_rest = true;
                    char *token_rest = token;
                    token = strtok_r(token_rest, " :\t\r\n", &token_rest);
                }
                strcpy(parsed_msg.params[counter - 1], token);
                printf("token 1 is %s with length %ld\n", token, strlen(token));
            }
            else
            {
                /* Param takes up rest of message */
                /* Accumulate the rest of param into params[counter - 1] */
                printf("BREAK 1\n");
                char *param_so_far = parsed_msg.params[counter - 1];
                int concat_param_len = strlen(param_so_far) 
                                        + strlen(token) + 1;
                char *concat_param = malloc(sizeof(char) * concat_param_len);
                concat_param = strcat(strcat(param_so_far, " "), token);
                free(parsed_msg.params[counter - 1]);
                // size_t new_param_size = sizeof(char) * concat_param_len;
                printf("BREAK 2\n");
                parsed_msg.params[counter - 1] = malloc(sizeof(char) * MAX_STR_LEN);
                printf("BREAK 3\n");
                strcpy(parsed_msg.params[counter - 1], concat_param);
                printf ("token 2 is %s\n", token);
            }
        }

        if (!param_is_rest)
        {
            /* Counter only increments when param does not take up the rest of
            the message */
            counter++;
        }
    }

    parsed_msg.num_params = param_is_rest ? counter : counter-1;
    printf ("Num of params is %d\n", parsed_msg.num_params);
    return parsed_msg;
}