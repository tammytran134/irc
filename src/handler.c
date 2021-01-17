#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "handler.h"
#include "reply.h"

bool sameStr(char* s1, char* s2) {
    return strcmp(s1, s2) == 0;
}

message parseMsg(char *msgBuffer) {
    char *token;
    char *rest = msgBuffer;

    message parsedMsg = {"", {}};
    int counter = 0;
    while (token = strtok_r(rest, " ", &rest)) {
        if (counter == 0)
            parsedMsg.command = token;
        else
            parsedMsg.params[counter - 1] = token;
    }

    return parsedMsg;
}

void processMsg(int clientSocket, char *clientHostname, message msg) {
    /* Execute parsed message */
    char *replyMsg;
    char *replyCode;
    if (sameStr(msg.command, "NICK")) {
        char *nick = msg.params[0];
        // TODO: add to nicks hashtable (contains socket information)
    }
    else if(samesStr(msg.command, "USER")) {
        char *username = msg.params[0];
        char *realName = msg.params[3];
        replyCode = RPL_WELCOME;
        char hostname[101];
        gethostname(hostname, 101);
        clientHostname = "foo.example.com";     // passed in from main.c
        char *replyMsg;
        sprintf(replyMsg,
            "%s %s %s :Welcome to the Internet Relay Network %s!%s@%s\r\n",
            hostname,
            replyCode,
            username,
            username,
            username,
            clientHostname
        );
        send(clientSocket, replyMsg, strlen(replyMsg), 0);
    }

    return;
}