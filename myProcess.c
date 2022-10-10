/*
Group F
Julius OLAIFA
CS 4323 Fall 2022

This function receives a process id and reads from a message Queue
It then processes input from user to save, summarize or return a 2D array to the server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "pipe.c"
#include "messageQ.c"

int nrows = 400;
void process_request(int pid){

    int i = 0;
    char* line;
    char** readArray = (char**)malloc(nrows * sizeof(char *));

    /* Dynamically create a 2D array */
    for(int i = 0; i < nrows; i++)
        readArray[i] = malloc(sizeof(char));

    /* Read from the message queue while there is a message
      into the created Array */
    while((line = msgQ_receive(pid)) != NULL) {
       readArray[i] = line;
       i++;
    }

    /* Handle the request of pipe */
    int input = atoi(readpipe(pid));
    if(input == 1) {
      writepipe(readArray);
    }else if(input == 2) {
      save(readArray);
    }else if(input == 3) {
      printf("Summary %d rows\n", no_of_rows(readArray))
    }else{
      printf("Error");
    }
}

/* save(char **): saves the array into a text file */

void save(char** arr) {
   output_file = fopen("filtered.txt", "w");
       if (NULL == output_file) {
       printf("file can't be opened \n");
   }
  int rows = no_of_rows(arr);
  for(int i = 0; i < rows; i++) {
    fprintf(output_file,"%s\n",content_cpy);
  }
  fclose(output_file);
}

/* no_of_rows(char **): returns an int
the number of rows in a 2D array */

int no_of_rows(char** arr) {
  int count = 0;
  for(int i = 0; i < nrows; i++) {
    if(arr[i] != NULL) {
      count++;
    }
  }
  return count;
}
