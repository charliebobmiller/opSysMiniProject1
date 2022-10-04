//include stuff n junk
#ifndef DEBUG
#define DEBUG 1
        #endif
#ifndef stdio
#include <stdio.h>
        #endif
#ifndef string
#include <string.h>
        #endif
#ifndef stringArray
#include "stringArray.c"
        #endif
        //windows doesnt know what this is
// #ifndef unistd
// #include <unistd.h>
//         #endif
#define MSG_SIZE_LIMIT 512 //the maximum length of a text string in a mail parcel
#define MSG_QUEUE_SIZE 10 //the number of mail parcels a mailbox can have 
#define MAILBOX_LIMIT_INITIAL 20



//REMOVE ON UNIX
int getpid() {
    return 14;
}

//structs:
typedef struct {//MAIL
    char* text;
    int destPID;
    int senderPID;

} mail; //a vessel that messages can be passed in

typedef struct {//MAILBOX
    int mailboxNumber; //should match the index in post_office
    int messageSizeLimit; //i.e. 4096 characters per message
    int address_pid; //pid of first process
    //queue stuff don wrry bout it
    mail* inbox;
    int queueSize;
    int numInQueue;
    int frontIndex;
    int rearIndex;
    int nextIndex;

} mailbox; //a place where mail lives

typedef struct {//post office
    int numMailboxes;
    int maximum_mailboxes;
    mailbox* mailboxes; //a pointer to the first mailbox. 

} post_office;



//construct/recycle
void mailbox_newMail(mail* m);
void mailbox_destroyMailbox(mailbox mb);
void mailbox_destroyMail(mail m);


int post_office_hasSpace(post_office p) {
    //check if post office exists

    //check if there is space
    return 1;
}

void post_office_expand(post_office p) {

}

void post_office_destroy(post_office p) {

    //do i need to destroy each mailbox here? yep. probably
    mailbox mb;
    for (int i = 0; i < p.numMailboxes; i++) {
        mb = *(p.mailboxes + i);
        mailbox_destroyMailbox(mb);

    }
    //each mailbox and everything in it is destroyed.

    //free mailboxes
    free(p.mailboxes);
}




////////////////////
//construct/recycle 
////////////////////

void mailbox_newMail(mail* m) {
    m->destPID = -1;
    m->senderPID = -1;
    m->text = calloc(MSG_SIZE_LIMIT,sizeof(char) );
}

void mailbox_destroyMailbox(mailbox mb) {
    //destroy each mail

    //destroy pointer to first mail 
}

void mailbox_destroyMail(mail m) {
    //:0
}