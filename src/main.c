/*
 *
 *  chirc: a simple multi-threaded IRC server
 *
 *  This module provides the main() function for the server,
 *  and parses the command-line arguments to the chirc executable.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "log.h"
#include "reply.h"

typedef struct msg {
    char* msg;
    int counter;
} msg_t;

msg_t recv_msg (char* buf, msg_t rmsg, int client_socket) 
{
    char c;
    send(client_socket, "it comes here1\n", 15, 0);
    printf("it comes here2\n");
    for (int i = 0; i < strlen(buf); i++)
    {
        c = buf[i];
        printf("it comes here\n");
        if (rmsg.counter == 512) {
            char copy_msg[512];
            strcpy(copy_msg, rmsg.msg);
            //process_msg(copy_msg);
            //send(client_socket, copy_msg, strlen(copy_msg), 0);
            char *new_msg = (char *) malloc (sizeof (char) * 512);
            rmsg.msg = new_msg;
            rmsg.counter = 0;
            return rmsg;
        }
        else 
        {   
            rmsg.msg[rmsg.counter] = c;
            if (c == '\n') {
                if (rmsg.msg[rmsg.counter-1] == '\r') {
                    char copy_msg[strlen(rmsg.msg)];
                    strcpy(copy_msg, rmsg.msg);
                    //process_msg(copy_msg);
                    //send(client_socket, copy_msg, strlen(copy_msg), 0);
                    char *new_msg = (char *) malloc (sizeof (char) * 512);
                    rmsg.msg = new_msg;
                    rmsg.counter = 0;
                }
            }
            else {
                rmsg.counter++;
            }
            //copy char to correct place in msg
        }
    }
    return rmsg;
}


// hashtable nicks

// struct params {
//     field no_param;
//     char array param;
//     char int reply;
// }

// handler(cmd) {
//     NICK -> params
//     PRIVMESSAGE -> params

//     switch (cmd) {
//         case 'NICK':    // p1, p2  p3
//             struct params nick:

//     }
// }

// process_msg(msg) {
//     // 
//     NICK 
//     USER


//     // parse command and parameter
// }


int main(int argc, char *argv[])
{
    int opt;
    char *port = "6667", *passwd = NULL, *servername = NULL, *network_file = NULL;
    int verbosity = 0;

    while ((opt = getopt(argc, argv, "p:o:s:n:vqh")) != -1)
        switch (opt)
        {
        case 'p':
            port = strdup(optarg);
            break;
        case 'o':
            passwd = strdup(optarg);
            break;
        case 's':
            servername = strdup(optarg);
            break;
        case 'n':
            if (access(optarg, R_OK) == -1)
            {
                printf("ERROR: No such file: %s\n", optarg);
                exit(-1);
            }
            network_file = strdup(optarg);
            break;
        case 'v':
            verbosity++;
            break;
        case 'q':
            verbosity = -1;
            break;
        case 'h':
            printf("Usage: chirc -o OPER_PASSWD [-p PORT] [-s SERVERNAME] [-n NETWORK_FILE] [(-q|-v|-vv)]\n");
            exit(0);
            break;
        default:
            fprintf(stderr, "ERROR: Unknown option -%c\n", opt);
            exit(-1);
        }

    if (!passwd)
    {
        fprintf(stderr, "ERROR: You must specify an operator password\n");
        exit(-1);
    }

    if (network_file && !servername)
    {
        fprintf(stderr, "ERROR: If specifying a network file, you must also specify a server name.\n");
        exit(-1);
    }

    /* Set logging level based on verbosity */
    switch(verbosity)
    {
    case -1:
        chirc_setloglevel(QUIET);
        break;
    case 0:
        chirc_setloglevel(INFO);
        break;
    case 1:
        chirc_setloglevel(DEBUG);
        break;
    case 2:
        chirc_setloglevel(TRACE);
        break;
    default:
        chirc_setloglevel(TRACE);
        break;
    }

    /* Your code goes here */
    int server_socket;
    int client_socket;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage *client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_storage);
    int yes = 1;
    int numbytes;
    char *buf = (char *) malloc (sizeof (char) * 100);;
    char *msg = (char *) malloc (sizeof (char) * 512);
    msg_t rmsg = {"", 0};
    rmsg.msg = msg;

    //char *msg = ":bar.example.com 001 user1 :Welcome to the Internet Relay Network user1!user1@foo.example.com\r\n";

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 

    if (getaddrinfo(NULL, port, &hints, &res) != 0) 
    {
        perror("getaddrinfo() failed");
        exit(-1);
    }

    for (p = res; p !=NULL; p = p->ai_next)
    {
        if ((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("socketfailed");
            continue;
        }
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
        {
            perror("setsockopt() failed");
            continue;
        }
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("bind() failed");
            close(server_socket);
            continue;
        }
        if (listen(server_socket, 5) == -1)
        {
            perror("listen() failed");
            close(server_socket);
        }
        break;
    }

    free(res);

    if (p == NULL)
    {
        perror("Couldn't find a socket to bind to");
        exit(-1);
    }

    while(1)
    {
        client_socket = accept(server_socket, (struct sockaddr *) client_addr, &sin_size);
        if ((numbytes = recv(client_socket, buf, strlen(buf), 0)) == -1) 
        {
            perror("recv() failed");
            exit(1);
        }
        buf[numbytes] = '\0';
        send(client_socket, buf, strlen(buf), 0);
        printf ("%s", buf);
        rmsg = recv_msg(buf, rmsg, client_socket);
        char *msg1 = ":bar.example.com 001 user1 :Welcome to the Internet Relay Network user1!user1@foo.example.com\r\n";
        send(client_socket, msg1, strlen(msg1), 0);
    }

    close(server_socket);

    return 0;
}

