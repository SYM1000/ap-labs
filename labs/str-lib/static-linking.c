#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);


int main(int argc, char **argv) {
    if(strcmp(argv[1], "-add") == 0){
        if(argc != 4){
            printf("Failed adition\n");
            exit(0);
        }
        
        printf("Initial Lenght      : %d\n", mystrlen(argv[2]));
        char *newString = mystradd(argv[2], argv[3]);
        printf("New String          : %s\n", newString );
        printf("New String          : %d\n", mystrlen(newString));


    }else if(strcmp(argv[1], "-find") == 0){
        if(argc != 4){
            printf("Failed find\n");
            exit(0);
        }
    
        int pos = mystrfind(argv[2], argv[3]);
        printf("[%s] string was found at [%d] position\n", argv[3], pos);

    }else{
        printf("Input Error");
        exit(0);
    }
}

