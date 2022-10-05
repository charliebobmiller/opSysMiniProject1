//2nd attempt at message queue. this time with more testing as i develop

#include "messageQ.h"

//primary functions

//send a message to a mailbox (mailbox belongs to PID)
int msgQ_send(int pid,char* message);
//receive a message from a mailbox 
char* msgQ_receive(int pid);

//message queue implementation
int mailbox_enqueue(mailbox* mb, char* message);
char* mailbox_dequeue(mailbox* mb);

//subroutines
mailbox* mailbox_find(int pid);
mailbox* mailbox_new(int pid);
mail mailbox_takeMail(mailbox* mb);
void mailbox_updateIndex(mailbox* mb);
int mod(int a, int b) {
    a = a % b;
    if(a < 0) return (a+b);
    return a;
}


//boolean
int mailbox_hasMailbox(int pid);
int mailbox_isFull(mailbox mb);
int mailbox_hasMail(mailbox* mb);
int mailbox_exists(mailbox mb);
int mailbox_validateQueue(mailbox mb);
int mailbox_mailExistsAt(mailbox mb,int index);


//this will end up mmap'd. doing a structure here so its easy to plop in mmem. implement mmap with like,
//an int - then a structure, then i'll know i can use structure in mmap
static post_office* local = NULL;
//used to be: //static post_office local = {0 , MAILBOX_LIMIT_INITIAL, NULL};
//but
#define PO_FILE "/Post_office_sharedMem"



///////////////////////////
/////////////driver main
int main() {
    int process1 = 14; //this is a fake pid because i'm not testing on processes yet
    char* buf = malloc(sizeof(char) * 255);
    int stat;

    //send message to process1
    for(int i = 1; i < 20; i++) {
        sprintf(buf,"message#%d",i);
        stat =  msgQ_send(process1,buf);
        if (stat >= 0)
            printf("sent '%s' with return code:%d\n",buf,stat); //if -1, try to resend
        else if (stat < 0)
            printf("send failed. message '%s' was not sent; return code:%d\n",buf,stat); //if -1, try to resend

        
    }
    printf("\n________________\ndone sending.\n\n");
    
    //recieve messages from process1 mailbox
    char* msg = msgQ_receive(process1);
    if (msg == NULL)
        printf("couldnt receive.\n",msg);
    else 
        printf("first recieved: [%s]\n",msg);
    while( mailbox_hasMail(mailbox_find(process1)) == 1) {

        buf = msgQ_receive(process1);
        printf("recieved: [%s]\n",buf);
    }
    printf("\n________________\ndone recieving.\n\n");
    

}











///////////////////
//primary functions
///////////////////


//send a message to a mailbox (mailbox belongs to PID)
int msgQ_send(int pid,char* message) {
    if (mailbox_shmemExists() == 0) { //then we need to establish it. 
	mailbox_shmemCreate();
    }


    mailbox* mb;
    //see if there is a mailbox ready to send to.
    if (mailbox_hasMailbox(pid) == 0) {
        //create mailbox
        mb = mailbox_new(pid);
    }
    else {
        //get mailbox associated with PID
        mb = mailbox_find(pid);
        if (mb->address_pid == -1) {//check to see if we found it
            if(DEBUG) printf("unable to find mailbox with that pid");
            return 0;
        }
    }

    //check if mailbox is full
    if (mailbox_isFull(*mb)) 
        {return -1; if(DEBUG)printf("mailbox was full.\n");}
    else 
        return mailbox_enqueue(mb,message); //put mail in mailbox..returns success
    

}

//receive a message from a mailbox 
char* msgQ_receive(int pid) {//returns message if one exists, NULL otherwise.
    if (mailbox_shmemExists() == 0) { //then we need to establish it. 
	mailbox_shmemCreate();
    }

    mailbox* mb;
    
    //check to see if mailbox exists 
    if (mailbox_hasMailbox(pid) == 1)
        mb = mailbox_find(pid);
    else 
        return NULL;//null means receive failed

    //check to see if there's a message
    if (mailbox_hasMail(mb))
        //return message
        return mailbox_dequeue(mb);
    else {
        if(DEBUG) printf("no mail to receive"); 
        return NULL;//no mail to receive
        //maybe do a wait() here? or a sleep?
    }


}


///////////////////////////////////
////message queue implementation
///////////////////////////////////

int mailbox_enqueue(mailbox* mb, char* message) {
    //check that mailbox exists
    if (mailbox_exists(*mb) == 0)
        //return 0 if it doesnt
        return 0;
    
    //check queue for space:
    if (mailbox_isFull(*mb) == 1)
        //return 0 if it is full
        return 0;

    ///////////////////////////////////
    ///////////put message in the queue
    
    //package the message in a mail object
    mail m = {NULL,mb->address_pid,getpid()}; //initialize 
    m.text = malloc(sizeof(char) * MSG_SIZE_LIMIT); //memory for string
    strcpy(m.text,message); //put text thar

    //place this mail at the next place in mailbox queue
    *(mb->inbox + mb->nextIndex) = m; 

    //update queue stuff
    mb->numInQueue++;
    mb->rearIndex = mb->nextIndex;
    mb->nextIndex = mod(mb->nextIndex + 1, mb->queueSize);
    
    return 1;
}


char* mailbox_dequeue(mailbox* mb) {
    //check that mailbox exists
    if (mailbox_exists(*mb) == 0)
        //return 0 if it doesnt
        return 0;
    
    //check queue for space:
    if (mailbox_hasMail(mb) == 0)
        //return 0 if it has nothing
        return 0;

    ///////////////////////////////////
    ///////////put message in the queue
    
    //take mail from mailbox
        
        //copy mail from next spot in queue. 
        mail m = *(mb->inbox + mb->frontIndex);

        //reset mail location in mailbox
        mailbox_newMail( (mb->inbox + mb->frontIndex) );
        mb->numInQueue--; //update count
        
        //go to next spot in queue if theres mail there. otherwise chill here
        if (mailbox_mailExistsAt(*mb,mod(mb->frontIndex + 1,mb->queueSize)) == 1) 
            mb->frontIndex = mod(mb->frontIndex + 1,mb->queueSize); 
        //else do nothing

        //check that the queue isn't all caddywompus
        if(mailbox_validateQueue(*mb) == 0) {
            printf("unable to resove mail queue conflict\n");
        }
    
    char* msg = m.text;
    //check that postage is legit
    if (m.destPID != mb->address_pid) {
        return 0;//address on letter doesnt match mailbox. FIXME so that this case is handled
    }

    //check that we're supposed to receive this
    if (m.destPID == getpid()) {
        return msg; //return message from mail
    }
    else {
        return 0; //we're not supposed to receive this. requeue it?
    }
}
























/////////////
//subroutines
/////////////


mailbox* mailbox_find(int pid) {
    for (int i = 0; i < local->numMailboxes; i++) {
        if (local->mailboxes[i].address_pid == pid) return (local->mailboxes + i);
    }

    return NULL;
    
}





int  mailbox_shmemExists() {
	return (local != NULL);
}


void mailbox_shmemCreate() {
	//do shm_open
	int fileDesc = shm_open(PO_FILE, O_RDWR | O_CREAT | O_TRUNC, 00666);
	//do mmap to local
	local = (post_office*) mmap(NULL, sizeof(post_office*), PROT_READ|PROT_WRITE, MAP_SHARED, fileDesc, 0);
	//i am hoping that 
	
}








mailbox* mailbox_new(int pid) {

    //do post office stuff
    if (post_office_hasSpace(*local) == 0) 
        post_office_expand(*local);
    if (local->mailboxes == NULL) 
        local->mailboxes = malloc(sizeof(mailbox) * local->maximum_mailboxes);
    
    //work with the current mailbox
    mailbox* newbox = (local->mailboxes + local->numMailboxes);
    //update count
    newbox->mailboxNumber = local->numMailboxes;
    local->numMailboxes++;
    
    //update mailbox list
    newbox->address_pid = pid;
    newbox->messageSizeLimit = MSG_SIZE_LIMIT;
    newbox->queueSize = MSG_QUEUE_SIZE;
    newbox->numInQueue = 0;

    //queue index stuff
    newbox->frontIndex = 0;
    newbox->nextIndex = 0;
    newbox->rearIndex = 0;

    //create place in memory for mail pointers
    newbox->inbox = malloc(sizeof(mail) * MSG_QUEUE_SIZE);

    //initialize each mail 
    mail* thisone;
    for (int i = 0; i < newbox->queueSize; i++){
        mailbox_newMail( (newbox->inbox + i) );
    }

    return newbox;

}











































////////////////
//BOOLEAN HELPERS
////////////////


int mailbox_mailExistsAt(mailbox mb,int index) {
    if ( (mb.inbox + index)->destPID == -1)
        return 0;
    else 
        return 1;
}




int mailbox_exists(mailbox mb) {return mailbox_hasMailbox(mb.address_pid);}
int mailbox_hasMailbox(int pid) {

    //loop through every mailbox in the post office
    for (int i = 0; i < local->numMailboxes; i++) {
        if (  (local->mailboxes + i)->address_pid == pid  ) //we found a mailbox with the pid
            return 1;//it does have a mailbox.

    }
    return 0;
}


int mailbox_isFull(mailbox mb) {
    if (mb.queueSize <= mb.numInQueue)
        return 1;
    else return 0;
}
int mailbox_hasMail(mailbox* mb) {
    if (mb == NULL) return 0;

    return (mb->numInQueue > 0);
}


int mailbox_validateQueue(mailbox mb) {
    //rotates right starting from front index, looks for a hole in the queue.
    //does not have an error if there is one remaining queue spot in the front
    //i.e. [* * * * * * _] is okay, [_ * * * * *] is okay, so is [* * * _ * *]
    //but [_ * * _ _ * _] is not okay.

    int n;
    for (int c = 0; c < (mb.queueSize - 2); c++) {
        n = mod( (mb.frontIndex + c) , mb.queueSize );
        //starting from the head, if we encounter a filled mail position
        if (mailbox_mailExistsAt(mb,n) == 1)
            //then an empty
            if (mailbox_mailExistsAt(mb,n+1) == 0)
                //then, skipping spaces, try to find another filled spot- 
                for (int k = mod( (n+2) , mb.queueSize); k < mb.queueSize; k++) {//go from the space we found, onwards
                    if (mailbox_mailExistsAt(mb,k) == 1)
                        //it's a problem. queue should be continuous
                        return 0;
                    else continue; //it was not occupied, so keep going
                }
            
    }
    return 1; //passed test, valid queue.
}



