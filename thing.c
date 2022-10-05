#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h> 
#include <stdio.h>

int main( ) {
    
    int size = 1024;
    char* name   = "test";

    int fd  = shm_open(name, O_RDWR|O_CREAT|O_TRUNC, 0600 );
    ftruncate(fd, size);
    char *ptr = (char *) mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    if (fork()) { // parent
        ftruncate(fd, size); // cut the file to a specific length
        char* msg = (char*)malloc(50*sizeof(char));
        sprintf(msg, "parent process with id %d wrote this in shared memory",getpid()); // copy this in msg
        strcpy((char*)ptr,msg); // copy msg in ptr (the mmap)
        munmap(ptr,size); // parent process no longer has access to this memory space
        shm_unlink(name); // parent process is no longer linked to the space named "test"
    } else {
        sleep(0.00001);
        printf("Inside child process %d :  %s\n", getpid(), ptr);
        munmap(ptr,size);
        exit(0);
    }
}
