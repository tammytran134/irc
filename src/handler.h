#ifndef CHIRC_HANDLER_H_
#define CHIRC_HANDLER_H_

typedef struct msg {
    char* command;
    char* params[15];
} message;

/* IRC server's message handler helper functions */
message parseMsg(char *buffer);
void processMsg(int clientSocket, char *clientHostname, message msg);

#endif