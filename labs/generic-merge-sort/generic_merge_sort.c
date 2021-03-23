#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>


char *arrayLines[100];
char *helperValues[100];


void getMerge(char *unsortedFile);

void merging(int low, int mid, int high);
void sort(int low, int high);



int main(int argc, char **argv)
{
    if(argc == 2){
        if(strcmp(argv[1] , "-n") == 0 || strcmp(argv[1] , "-o") == 0){
            printf("Input error:\n");
            printf("For numbers input must be: -n [input file].txt\n");
            printf("For Strings input must be: [input file].txt\n");
	        return 0;
        }
        //merge de strings
        getMerge(argv[1]);

    
    }else if(argc == 3){
        //merge para numeros
        getMerge(argv[2]);

    }else{
        printf("Input error:\n");
        printf("For numbers input must be: -n [input file].txt\n");
        printf("For Strings input must be: [input file].txt\n");
	    return 0;
    }
    
}



void getMerge(char *unsortedFile){

    //Read files
    FILE *inputFile;
    
    inputFile  = fopen(unsortedFile, "r"); // read only
        
    // Validate the existance of the file
    if (inputFile == NULL) {
        printf("Error. %s not found\n", unsortedFile);
        exit(0);
    }

    //program logic
    int i=0;
    int num;
    char line[100];
    char allLines[100000]="";


    while(fgets(line, sizeof(line), inputFile) ){
        //numbers[i] = atoi(line);
        strcat(line, " ");
        strcat(allLines, line);
        i++;
    }

    //Fill array with strings
    int pos=0;
    arrayLines[pos] = strtok(allLines," ");
    while(arrayLines[pos]!=NULL){
        arrayLines[++pos] = strtok(NULL," ");
    }

    int max = 99;
    int min = 0;

    sort(min, max);


    char *uno = "sorted_";
    char *dos = malloc(strlen(unsortedFile) + 10);  //  Allocate memory
    strcat(dos, uno);
    strcat(dos, unsortedFile);
    free(dos);

    //get output file
    FILE *outputFile;

    outputFile = fopen(dos, "w");

    if (outputFile == NULL) {
        printf("Error. %s not found\n", dos);
        exit(0);
    }
   

    //Save to file
    for(i = 0; i <= max; i++)
        fprintf(outputFile, "%s", arrayLines[i]);

    printf("Results file can be found at ./%s\n", dos);

    
    fclose(inputFile);
    fclose(outputFile);
}


void merging(int low, int mid, int high) {
   int l1, l2, i;

   for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
       if( atoi(*(arrayLines + l1)) ){
           if( atoi(*(arrayLines + l1)) <= atoi( *(arrayLines + l2))  ){
            *(helperValues + i) = *(arrayLines + l1++);
        }else{
            *(helperValues + i) = *(arrayLines + l2++);
        } 
       }else{
           if( strcmp(*(arrayLines + l1), *(arrayLines + l2) ) <= 0){
            *(helperValues + i) = *(arrayLines + l1++);
        }else{
            *(helperValues + i) = *(arrayLines + l2++);
        }
       }

            
   }
   
   while(l1 <= mid)    
      

    *(helperValues + i++) = *(arrayLines + l1++);

   while(l2 <= high)   

        *(helperValues + i++) = *(arrayLines + l2++);

   for(i = low; i <= high; i++)

        *(arrayLines + i) = *(helperValues + i);
}


void sort(int low, int high) {
   int mid;
   
   if(low < high) {
      mid = (low + high) / 2;
      sort(low, mid);
      sort(mid+1, high);
      merging(low, mid, high);
   } else { 
      return;
   }   
}