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
#define STRINGARRAY_GROW_AMT 16 //change this for smaller systems
///////////
//stringArray typedef
    
    //type definition
    typedef struct {
        int numStrings;
        char** headString;
        int arraySize;

    } stringArray;

    stringArray strArr(); //make space for this string array
    void strArr_append(stringArray* s, char* string); //dynamically add a string to end of array
    void strArr_cat(stringArray* dst, stringArray src); //smash two arrays together starting with dst
    void strArr_grow(stringArray* s, int growby); //give more memory for array to play with
    void strArr_shrink(stringArray* s); //take away unused memory from array
    void strArr_destroy(stringArray* s); //free all memory associated with this stringArray
    void strArr_remove(stringArray* s, int index); //remove indexed member from stringArray
    void strArr_insert(stringArray* s, char* string, int index); //insert string into a spot at index
    int strArr_find(stringArray s, char* searchString); //returns the index to first occurrance of searchstring or -1 for notfound



    stringArray strArr() {
        stringArray a;
        a.numStrings = 0;
        a.arraySize = 0;
        a.headString = NULL; //originally no strings associated


        return a;
    }


    void strArr_append(stringArray* s, char* string) {
        //grow if not enough room
        if (s->arraySize <= s->numStrings) strArr_grow(s,STRINGARRAY_GROW_AMT);


        //allocate space for this string in the array
        char* tmem = malloc(sizeof(char) * strlen(string));
        if (tmem != NULL)  s->headString[s->numStrings] = tmem;
        else printf("Malloc error in strArr_append()\n");
        
        //copy string into space
        strcpy(s->headString[s->numStrings],string);

        //update string count in array
        s->numStrings++; 
    }

    void strArr_cat(stringArray* dst, stringArray src) { //concatenate two string arrays

        //make space for all of src in dst
        strArr_grow(dst,src.numStrings);

        //add strings from src to dst
        for (int i = dst->numStrings; i < (dst->numStrings + src.numStrings) ; i++ ) {
            dst->headString[i] = src.headString[i - (dst->numStrings)];
        }

        //update numstrings
        dst->numStrings = dst->numStrings + src.numStrings;

    }

    void strArr_grow(stringArray* s, int growby) {

        //attempt to reallocate memory with new preferred size
        char** new = realloc(s->headString, ( sizeof(char**) * (s->arraySize + growby )));
        if (new == NULL) printf("error: unable to allocate memory to grow string array.\n"); //leak prevention
        else s->headString = new;

        s->arraySize += growby; //update array size
        
        //set unused pointers to null
        for(int i = s->numStrings; i < s->arraySize; i++) {
            s->headString[i] = NULL;
        }

    }

    void strArr_shrink(stringArray* s) {
        printf("shrink unimplemented... f your heap anyway\n");
    }

    void strArr_print(stringArray s) {
        if (s.numStrings > s.arraySize) {
            printf("[uninitialized string array]\n");
            return;
        }

        printf("\n\n[");
        for (int i = 0; i < s.numStrings; i++) {
            printf("%s",s.headString[i]);
            if (i < s.numStrings-1) printf(", ");
        }
        printf("]\n");
    }


    
    void strArr_destroy(stringArray* s) {
        //we must destroy all parts of this. 
        //each string
        for(int i = 0; i < s->numStrings; i++) {
            free(s->headString[i]);//free the memory that was allocated for every string in this array
        }

        //free the string array
        free(s->headString);
    }

    void strArr_remove(stringArray* s, int index) {
        //remove an element and compress array
        if ( (index >= s->numStrings) || (index < 0) ) return; //bad index

        free(s->headString[index]); //removed
        s->numStrings--; //count down 1 string now to implement fillin logic. 
        for(int i = index; i < s->numStrings; i++) {//wont access out of element because numstrings reduced above
            //file in the rest
            s->headString[i] = s->headString[i+1];
        }

        //SHRINK??
    }

    void strArr_insert(stringArray* s, char* string, int index) {
        //check to grow
        if (s->arraySize <= s->numStrings) strArr_grow(s,STRINGARRAY_GROW_AMT);

        //append if at the end
        if (index >= s->numStrings) index = s->numStrings;

        //prepend for small indexes
        if (index <= 0) index = 0;
        
        //allocate space for this string in the array
        char* tmem = malloc(sizeof(char) * strlen(string));
        if (tmem == NULL) printf("Malloc error in strArr_insert()\n");
        else strcpy(tmem,string);
        

        //scoot everything up a spot
        for (int i = s->numStrings; i > index; i--) {
            s->headString[i] = s->headString[i-1]; 
        }

        //install new member
        s->headString[index] = tmem;

        //update number of strings
        s->numStrings++;



    }

    int strArr_find(stringArray s, char* searchString) {
        //loop thru comparing strings
        for (int i = 0; i < s.numStrings; i++) {
            if ( strcmp(s.headString[i],searchString) == 0 ) 
                return i;//first occurrance found
        }
        return -1; //not found
        //maybe update this to return more than one index, or provide a starting place?

    }
    