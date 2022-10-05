#include <sys/mman.h>//shm_open, mmap
#include <fcntl.h> //helps shm_open with o constants
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define SECRET_LOCATION "/asdfgarden"
#define buffLength 255
#define FSIZE 255
int main() {


typedef struct {
	char* asdf;
	int length;
	

} thing;
	int cpid = fork();
	//parent
	if ( cpid != 0 ) {
		//create shared memory and story a message.
		int fileDesc = shm_open(SECRET_LOCATION,O_RDWR | O_CREAT | O_TRUNC, 00666);
		ftruncate(fileDesc,sizeof(thing*)); //this is like malloc?
		
		thing* rem = (thing*) mmap(NULL, sizeof(thing), PROT_READ|PROT_WRITE , MAP_SHARED, fileDesc, 0 /*offset*/);

		rem->asdf = malloc(sizeof(char) * 255);
		strcpy(rem->asdf,"qwerqwer");
		rem->length = 7;
		printf("P:okay i made a thing. It is located at: %d. \nits contents are: '%s' and [%d].\n",(int)rem,rem->asdf,rem->length);
		wait(NULL);

	}

	//child
	else {
		wait(NULL); //wait for parent to finish?

		int fileDesc = shm_open(SECRET_LOCATION,O_RDWR, 00666);
		ftruncate(fileDesc,sizeof(thing*)); //this is like malloc?

		thing* rem = (thing*) mmap(NULL, sizeof(thing), PROT_READ|PROT_WRITE , MAP_SHARED, fileDesc, 0 /*offset*/);

		printf("C:okay i made a thing. location: [%d]\nits contents are: '%s' and [%d].\n",(int)rem,rem->asdf,rem->length);

	}

}
