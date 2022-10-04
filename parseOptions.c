///////////includes
#ifndef stdio
    #include "stdio.h" 
    #endif
#ifndef string
    #include "string.h" 
    #endif
#ifndef stdlib
    #include "stdlib.h"
    #endif
#ifndef stringArray
    #include "stringArray.c"
    #endif
#ifndef ctype
    #include "ctype.h"
    #endif

#define file_options "data/options.txt"
#define DELIM ','
#define BUFFER_SIZE 255

//primary methods
    stringArray getFilenames();
    stringArray getSortKeys(char* filename);
//helper methods
    //given a string of things you want to separate, returns the next token separated by delimiters
    char* consumeNextToken(char** buffer,char delim); //given a string, split into two parts.
    int hasMoreTokens(char* buf);
    int isEOL(char inp);
    void trimEOL(char** str);
    void shiftstring(char** str, int shiftamt);

stringArray getFilenames() {
    stringArray sarr = strArr();
    FILE *fp; 
    fp = fopen(file_options,"r");
    char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
    char* token = malloc(sizeof(char) * BUFFER_SIZE);
    while (    ( fgets(buffer,BUFFER_SIZE,fp) ) != NULL) {//get whole row from options

        while ( hasMoreTokens(buffer)) {//separate rows into tokens
            strcpy(token,consumeNextToken(&buffer,DELIM));
            trimEOL(&token);
            strArr_append(&sarr, token );
        }

        break; //i only care about the first line
    }
    fclose(fp);
    free(token);
    return sarr;

}

stringArray getSortKeys(char* filename) {//gets the sort keys associated with a filename
    FILE *fp; 
    fp = fopen(file_options,"r");
    char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
    char* token = malloc(sizeof(char) * BUFFER_SIZE);

    //all we are given is filename. look in options for this filename and get column number
    int column = 0; 
    int foundIndex;
    if (fgets(buffer,BUFFER_SIZE,fp) != NULL) {//if we can read a line (file has top row)
        while (hasMoreTokens(buffer)) {//while there are remaining tokens in the line
            strcpy(token,consumeNextToken(&buffer,DELIM));//get the next token, makes buffer NULL if empty.
            trimEOL(&token);
            if( strcmp(token,filename) == 0) { //see if its the filename we want
                foundIndex = column;//record index
                break;//now we know the column
            }
            column++; //indicate next column
        }
    } 
    else {printf("ERROR: failed reading options.txt (FATAL)\n");exit(0);}

    //we know which column it is, now we just accumulate each member of the column. 
    stringArray sortkeys = strArr(); //
    while (fgets(buffer,BUFFER_SIZE,fp) != NULL) { //read all lines (starting from 2nd row)
        
        for (int i = 0; i < foundIndex; i++) //for each member of row before row we care about
            consumeNextToken(&buffer,DELIM); //consume all tokens before the one we care about

        //we care about this token, so append it to sort key array
        strcpy(token,consumeNextToken(&buffer,DELIM));
        trimEOL(&token);
        strArr_append(&sortkeys,token);

        continue;//continue to next line (discarding the rest of this line)
    }
    fclose(fp);
    free(token);
    free(buffer);
    return sortkeys;
}

int hasMoreTokens(char* buf) {
    if (*buf == '\0') return 0;
    return 1;
}

char* consumeNextToken(char** str,char delim) { //strip and return the next delimited string from buffer, 
    char* ret;

    if (str == NULL) printf("raw string was NULL pointer.\n"); 
    
        //search for first character that is NOT delim
        int delimIndex;
        int tokenStart = -1;
        int len = strlen(*str);
        for (int i = 0; i < len; i++) 
            if (*(*str + i) != delim) {//token beginning found
                tokenStart = i;
                break;
            }

        //if no character is found, there are no tokens
        if (tokenStart == -1) {
            *(*str + 0) = '\0'; //
            return NULL; 
        }

        else {//first character is found, 
            for (int i = tokenStart ; i < len; i++) {//search for DELIM

                //if delim is found
                if (*(*str + i) == delim) {
                    delimIndex = i;
                    //separate strings
                    //generate return string
                    ret = malloc(sizeof(char) * (delimIndex + 1)); 
                    for(int j = 0; j < delimIndex; j++) 
                        ret[j] = *(*str + j);
                    ret[delimIndex] = '\0';

                    //better, charlie. you learned. 
                    shiftstring(str,i+1); //shift string contents so the ith character becomes the first
                    
                    return ret;
                }
            }

            //if delim is not found
                //str only has one token

                //generate return string
                ret = malloc(sizeof(char) * (len-tokenStart)); 
                for(int j = tokenStart; j < len; j++) 
                    ret[j-tokenStart] = *(*str + j);
                ret[len] = '\0';

                //set str head to terminated to indicate that it's empty
                *(*str + 0) = '\0';
                return ret;
            
        }
        
    

}

void trimEOL(char** str) {
    //copy string that isnt EOL. 
    int len = strlen(*str);
    char* new = malloc(sizeof(char) * len);
    int c = 0;
    for (int i = 0; i < len; i++) { //copy all non-whitespace characters
        if ( 0 == isEOL(*(*str + i)) ) {new[c] = *(*str + i); c++;}
    }
    new[c] = '\0';
    strcpy(*str,new);
}

int isEOL(char a) {
    //probably a more efficient way to do this than comparing it to each possible 
    if (a == '\n');
    else if (a == '\r');
    else if (a == '\t');
    else if (a == '\v');
    else return 0;
    return 1;
}

//shift string contents so the ith character becomes the first
void shiftstring(char** str, int shiftamt) {
    int len = strlen(*str); 
    int i = 0;
    for(; i < (len - shiftamt); i++) {//loop through each position in the string
        *(*str + i) = *(*str + i + shiftamt);
    }
    *(*str + i) = '\0';
} 

// int main() {
//     printf("\ntesting parseOptions.txt parser...\n");
//     printf("getting filenames from options.txt...\n");
//     stringArray filenames = getFilenames();
//     printf("filenames:");
//     strArr_print(filenames);
    
//     for(int i = 0; i < filenames.numStrings; i++) {
//         printf("\ngetting sortkeys from filename..%s\n",filenames.headString[i]);
//         stringArray sortkeys = getSortKeys(filenames.headString[i]);
//         strArr_print(sortkeys);
//     }
    

// }