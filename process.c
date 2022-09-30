#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void process_request(char*, char*, char*);

int main(int argc, char *argv[])
{
  process_request(argv[1],argv[2],argv[3]);
  return 0;
}


void process_request(char *file_name, char *column_name, char *unique_value){

    FILE* input_file;
    FILE* output_file;
    char *buffer;
    size_t bufsize = 32;
    size_t characters;
    char *token;
    char *unq_tok;
    char *content_cpy;
    int col_num;
    struct stat sb;
    stat(file_name, &sb);

    char *file_contents = malloc(sb.st_size);
    buffer = (char *)malloc(bufsize * sizeof(char));
    content_cpy = malloc(sizeof(char) * 254);;

    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    input_file = fopen(file_name, "r");
    if (NULL == input_file) {
        printf("file can't be opened \n");
    }

    characters = getline(&buffer,&bufsize,input_file);
    output_file = fopen("filtered.txt", "w");
        if (NULL == output_file) {
        printf("file can't be opened \n");
    }
    token = strtok(buffer, ",");

    col_num = 0;
    while( token != NULL ) {
        if(!strcmp(token, column_name)) {
          break;
        }
        col_num++;
        token = strtok(NULL, ",");
    }


    while (fscanf(input_file, "%[^\n] ", file_contents) != EOF) {
          strcpy(content_cpy,file_contents);
          unq_tok = strtok(file_contents, ",");
          for(int i = 0; i < col_num; i++) {
             unq_tok = strtok(NULL, ",");
          }
          if(!strcmp(unq_tok, unique_value)) {
            fprintf(output_file,"%s\n",content_cpy);
            printf("> %s\n", content_cpy);
          }
    }

    fclose(input_file);

}