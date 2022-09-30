#ifndef stdio
#include <stdio.h>
        #endif
#ifndef string
#include <string.h>
        #endif
#ifndef stringArray
#include "stringArray.c"
        #endif
// #ifndef unistd
// #include <unistd.h>
//         #endif
typedef struct {
    int mailboxNumber;
    char* description;
    int messageSize; //i.e. 4096 characters per message
    int pidA; //pid of first process
    int pidB; //pid of second process
    stringArray queue;


} mailbox;

char* msgQ_receive(int pid) { //receive one message from the message queue

}

void msgQ_send(int pid, char* message) {

}

int mailbox_isFull(mailbox mb) {

}

int mailbox_isEmpty(mailbox mb) {
    
}