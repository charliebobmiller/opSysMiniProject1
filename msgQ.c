//ideally, the message queue works like sending a letter 
//therefore, there should be two functions externally well-known: 
//msgQ_send(PID, message)
//with this, all you need to do is send a message to a PID, 
//then its up to the process to check its mailbox. 

//msgQ_receive 
//this should be identical to checking the mailbox that goes to your house or apartment.
//just open the door, and stuff should be there. 
//since this is a computer, you can take one message out per call.
//should be poll-able.. maybe wait()?

//therefore, to implement this, we will need memory somewhere that is accessible,modifiable, expandable.
//lets just assume the a static variable will work for now, since i dont know any better. 
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
// #ifndef unistd
// #include <unistd.h>
//         #endif
#define MSG_SIZE_LIMIT 512 //the maximum length of a text string in a mail parcel
#define MSG_QUEUE_SIZE 10 //the number of mail parcels a mailbox can have 


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
    mail* queue;
    int queueSize;
    int frontIndex;
    int rearIndex;
    int nextIndex;

} mailbox; //a place where mail lives






//primary functions:
char* msgQ_receive(); //receive one message from the message queue
int msgQ_send(int pid, char* message); //send a message to a process. returns -1 if message send failed.

//queueing 
int queue_indexIsOccupied(mailbox mb, int indx) ;
mail mailbox_dequeue(mailbox mb) ;//takes mail off the queue
int mailbox_enqueue(mail m, mailbox mb); //puts mail on the queue, returns success


//helper functions:
mailbox mailbox_create(int pid);
int mailbox_numInQueue(mailbox mb);
int mailbox_isFull(mailbox mb);
int mailbox_isEmpty(mailbox mb);
int mailbox_hasRoom(mailbox mb);
int hasMailbox(int pid);
void mailbox_destroy(mailbox mb);
mailbox getMailbox(int PID);
int mailbox_PIDis(mailbox mb, int pid);
mail mail_createEmpty();

void mailbox_setNext(mailbox mb) ;//update head and next pointers
void mailbox_setFront(mailbox mb) ;//updates the front pointer




//this is a struct that we'll place in static memory so multiple processes can access it
//CHANGE THIS TO MMAP!!!!
//void* mmap(NULL, LENGTH, PROT_NONE, MAP_SHARED, )
typedef struct {
    int numMailboxes;
    mailbox* mailboxes; //a pointer to the first mailbox. 
} post_office;

//SHARED BETWEEN PROCESSES????
static post_office local = {0, NULL};


char* msgQ_receive(int pid) { //receive one message from the message queue
    //if there is no mailbox for this pid, create one. 
    if ( hasMailbox(getpid()) == 0 )
        mailbox_create(getpid());

    //get the most recent mail 
    mailbox mb = getMailbox(pid); //return mailbox for the target process
    mail m = mailbox_dequeue(mb); //take a mail off the queue 
    char* message = m.text;       //unpackage message

    //do extra stuff here 

    //return
    return message;


}

//attempt to enqueue a message in mailbox belonging to PID.
int msgQ_send(int pid, char* message) { 
    //return 1: message sent successfully. mailbox has remaining room.
    //return 0: message sent successfully. mailbox does not have remaining room. 
    //return -1: message was NOT sent. the mailbox was full. 
    //return -2: message was NOT sent. the message was too long.

    //check for null pointer
    if(message == NULL) printf("bad message. cannot send.\n");

    //check for long message.
    if (strlen(message) >= MSG_SIZE_LIMIT) return -2;

    //package message into a mail structure
    mail m = {message,pid,getpid()};
    
    //find mailbox for this pid if it exists
    mailbox mb;
    if ( hasMailbox(pid) == 0 ) 
        mb = mailbox_create(pid);
    else 
        mb = getMailbox(pid);

    //attempt to put the mail in the mailbox
    if (mailbox_isFull(mb) == 1) {

        if (DEBUG) printf("mailbox is full, cant put mail here.\n");
        return -1; 
    }
    else if (mailbox_hasRoom(mb) == 1) {
        int sces = mailbox_enqueue(m,mb);

        //check that enqueue succeeded.
        if (sces == 0) return -3; //enqueue failed.
        
        //check if space remains
        if (mailbox_hasRoom(mb) == 1) return 1;
        else return 0;
    }
    else {
        printf("undefined behavior.\n");//idkwtf??
        return -3;
    }



}

mailbox mailbox_create(int pid) {
    if (hasMailbox(pid) == 0)
        return getMailbox(pid);
    mailbox newbox;
    newbox.address_pid = pid; //mailbox is made for process at this PID to receive
    //set up mailbox with post office and stuff
    *(local.mailboxes + local.numMailboxes) = newbox;
    newbox.mailboxNumber = local.numMailboxes;
    local.numMailboxes++;
    newbox.messageSizeLimit = MSG_SIZE_LIMIT;

    newbox.queueSize = MSG_QUEUE_SIZE;
    newbox.frontIndex = 0;
    newbox.rearIndex = 0;
    for (int i = 0 ; i < newbox.queueSize; i++) {
        //create an empty mail in each spot
        *(newbox.queue + i) = mail_createEmpty();
    }//number of mail parcels in each mailbox wont change
    

}

int mailbox_enqueue(mail m, mailbox mb) { //returns success?
    if (mailbox_hasRoom(mb) == 0) return 0; //failed. no room.
    //otherwise, we assume theres space.

    //put the mail in the queue.
    if (queue_indexIsOccupied(mb,mb.nextIndex) == 1) return 0; //fail. occupied.
    *(mb.queue + mb.nextIndex) = m; //go for it

    //update the head/rear etc. in the mailbox queue.
    mailbox_setNext(mb);

}

mail mailbox_dequeue(mailbox mb) { //takes mail off the queue
    if (queue_indexIsOccupied(mb,mb.frontIndex)) {
        //return this mail, also dissociate it from the queue. 
        mail* f = (mb.queue + mb.frontIndex);
    }
    else {
        mail nullmail = mail_createEmpty();
        return nullmail;
    }
}   

int queue_indexIsOccupied(mailbox mb, int indx) {
    if (  (mb.queue + indx )->destPID == -1) return 0;
    else return 1;

}

mail mail_createEmpty() {
    mail nullmail = {NULL,-1,-1};
    return nullmail;
}

void mail_destroyMail(mail m) {
    printf("destroy mail subroutine is stubbed.\n");
}


mail mailbox_mailAt(mailbox mb, int indx) {
    return *(mb.queue + indx);
}
void mailbox_removeMail(mailbox mb, int indx) {
    printf("remove mail is stubbed.\n");
}


void mailbox_setNext
(mailbox mb) {//update head and next pointers
    //starting at rear, look for the next open spot
    int i = mb.rearIndex; 
    int n;
    for(int c = 0; c < mb.queueSize; c++) {
        n = ( i + c ) % mb.queueSize;//try this
        if (queue_indexIsOccupied(mb,n) == 0) 
            mb.nextIndex = n; //not occupied? thats the next.
        else mb.rearIndex = n; //occupied? drag the rear here

    }      
    
}


void mailbox_setFront(mailbox mb) {//updates the front pointer
    if ( queue_indexIsOccupied(mb,mb.frontIndex) == 1)
        return; //no need to update. already points to front
    //starting at rear, look for the next open spot
    int i = mb.frontIndex; 
    int n;
    for(int c = 0; c < mb.queueSize; c++) {
        n = ( i - c ) % mb.queueSize;//try this
        if (queue_indexIsOccupied(mb,n) == 0) 
            mb.frontIndex = n; //not occupied? thats the next.
        else mb.rearIndex = n; //occupied? drag the rear here

    }   
}

void mailbox_restructure(mailbox mb) {//correct a broken queue
    printf("restructure mailbox is stubbed\n");
}

int mailbox_isFull(mailbox mb) {
    if ( mailbox_numInQueue(mb) >= mb.queueSize ) return 1;
    else return 0;
}

int mailbox_hasRoom(mailbox mb) {
    if ( mailbox_isFull == 0 ) return 1;
    else return 0;
}

int mailbox_isEmpty(mailbox mb) {
    if (mailbox_numInQueue(mb) == 0) return 1;
    else return 0;
}

int mailbox_numInQueue(mailbox mb) {
    if(mb.rearIndex > mb.frontIndex) return (mb.rearIndex - mb.frontIndex + 1);
    else if (mb.rearIndex < mb.frontIndex) return (mb.rearIndex + 1) + (mb.queueSize - mb.frontIndex); //len(first part) + len(second part)
    else if (mb.rearIndex == mb.frontIndex) return 0;
    else return -1; //idkwtf
}

void mailbox_destroy(mailbox mb) {
    //get rid of the space for each parcel
    // for (int i = 0; i < mb.queueSize; i++) {
    //     free(  *(mb.queue + i)->text ); 
    // }

    //kill anything else

    //no mallocs were used in the creation of a mailbox. do i need to free memory now?
}



int hasMailbox(int pid) { //check to see if a process has a mailbox associated with it.
    if ( getMailbox(pid).mailboxNumber >= 0) return 1;
    else return 0;
}

mailbox getMailbox(int pid) {
    //find the mailbox associated with PID
    for (int i = 0; i < local.numMailboxes; i++) {
        if ( (local.mailboxes + i)->address_pid == pid)
            return *(local.mailboxes + i);
    
    }
    mailbox nullbox;
    nullbox.mailboxNumber = -1;
    return nullbox; //mailbox doesnt exist
}


int main() { //test this shit out :)
    int process1 = 14; //this is a fake pid because i'm not testing on processes yet
    msgQ_send(process1,"the first message.");

}