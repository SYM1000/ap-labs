#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>

#define REPORT_FILE "packages_report.txt"
#define SIZE 20000000 //tamano temporal de la HT TODO: Acutualizar con una funcion de rehasing para la HT

// Strucuture of packages
struct Package {
    char name[1000];
    char Date[1000];
    char InstallDate[1000];
    char lastUpdate[1000];
    char status[1000];
    char manager[1000];
    int numberUpdtaes;
    char removalDate[1000];
};

void analizeLog(char *logFile, char *report);
//int checkIfLineContainsWord(char line[1000], char package[]);
void createPackageFromLine(char line[1000], int *ScirptletCount, int *pacmanCount);
void insert( struct Package *myPackage);
void getPacmanPackagesReport(FILE *outputFile);
void getGeneralStats(int sciptletCount, int pacmanCount,FILE *inputFile, FILE *outputFile);
void printListOfPackages(FILE *outputFile);
void getOldest(FILE *outputFile);
void getNewest(FILE *outputFile);
bool checkExtension(char line[1000]);
struct Package *search(char key[1000]);


struct Package* hashArray[SIZE];
struct Package* item;


int string_hashCode(char key[1000]) {
    unsigned int hash = 0;

    for (int i = 0; i < strlen(key); i++) {
        hash = 31 * hash + key[i];
    }

    return hash % SIZE;
}




int main(int argc, char **argv) {

    if (argc != 5) {
	printf("Input error.\nInput must follow the follow order: -input [input file].txt -report [output file]");
	return 1;
    }else {

        if (!checkExtension(argv[2])){
            printf("Input error.\nInput file must have '.txt' extension\n");
	        return 1;
        }
    }


    analizeLog(argv[2], argv[4]);

    return 0;
}


void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);

    //Read files
    FILE *inputFile;
    FILE *outputFile;
    

    inputFile  = fopen(logFile, "r"); // read only
    outputFile = fopen(report, "w"); // write only
    
    // Validate the existance of the file
    if (inputFile == NULL || outputFile == NULL) {   
        printf("Error. %s not found\n", logFile); 
        exit(1);
    }

    int ScirptletCount = 0;
    int pacmanCount = 0;
    char line[1000];


    while( fgets(line, sizeof(line), inputFile) ){
        createPackageFromLine(line, &ScirptletCount, &pacmanCount);
        continue;
    }

    //get results data
    getPacmanPackagesReport(outputFile);
    getGeneralStats(ScirptletCount, pacmanCount, inputFile, outputFile);
    printListOfPackages(outputFile);


    printf("\nReport is generated at: [%s]\n", report);

    //Close all files
    fclose(inputFile);
    fclose(outputFile);
}


//This method checks if the given sentence contains the given word.
//Return 1 if word is founded.
int checkIfLineContainsWord(char line[1000], char package[]){
    
    char sentence[1000];
    strcpy(sentence, line);
    char* word = strtok(sentence, " ");

    while (word != NULL) {
        if(strcmp(word,package) == 0) {
            return 1;
            printf("Encontrado: %s\n", word);
        }
        word = strtok(NULL, " "); 
    }
    strcpy(sentence, line);

    return 0;
}


void createPackageFromLine(char line[1000], int *ScirptletCount, int *pacmanCount){

    char sentence[1000];
    strcpy(sentence, line);
    char* word = strtok(sentence, " ");
    int i = 1;

    //define variables for package
    char Date[1000];
    char Time[1000];
    char manager[1000];
    char status[1000];
    char name[1000];
    char version[1000];

    bool isNotAPackage = false;

    

    while (word != NULL) {
        isNotAPackage = false;

        if(i == 1){
            strcpy(Date, word);
        }else if (i == 2){
            //validacion de la hora y la fecha
            //Para conocer si es el primero o el segundo formato
            if(strchr(word, ':') != NULL){ //Si el formato [2019-03-22 21:24]
                strcpy(Time, word);
            }else{ //Si es el fromato [2020-04-09T18:35:11+0000]
               i++;
               continue;
           }
            
        }else if (i == 3){
            strcpy(manager, word);
            if( strcmp(manager, "[ALPM-SCRIPTLET]") == 0){
                *ScirptletCount =  *ScirptletCount+=1;

            } else if ( strcmp(manager, "[PACMAN]") == 0){
                ++*pacmanCount;
            }

        }else if (i == 4){
            //This is not a package;
            if (  strcmp(word, "installed") != 0 && strcmp(word, "removed") != 0 && strcmp(word, "upgraded") != 0){
                isNotAPackage = true;
                break;
            }
            strcpy(status, word);
        }else if (i == 5){
            strcpy(name, word);
        }else if (i == 6){
            strcpy(version, word);
        }

        word = strtok(NULL, " "); 
        i++; 
    }

    strcpy(sentence, line); //hacer que no se borre la linea


    if(isNotAPackage){
        return;
    }

    struct Package currentPackage;
    strcpy(currentPackage.Date, strcat(strcat(Date, " ") ,Time));
    strcpy(currentPackage.manager, manager);
    strcpy(currentPackage.status, status);
    strcpy(currentPackage.name, name);
    strcpy(currentPackage.InstallDate, Date);
    strcpy(currentPackage.lastUpdate, "");
    currentPackage.numberUpdtaes = 0;
    strcpy(currentPackage.removalDate, "");

    item = search(currentPackage.name);


    //Checar si existe en la hashtable
   if(item != NULL) {
      strcpy(item->status, currentPackage.status); //Actualziar el status del paquete

        //increase number of updates
      if( strcmp(currentPackage.status, "upgraded") == 0 ){
          ++item->numberUpdtaes;
          strcpy(item->lastUpdate, currentPackage.Date);
          
      }
    
        if( strcmp(item->status, "removed") == 0 ){
            strcpy(item->removalDate, currentPackage.Date);
      }
 
   } else {
       insert( &currentPackage );
   }
  
}
    
 void getPacmanPackagesReport(FILE *outputFile){
    int installedPackagesCount = 0;
    int removedPackagesCount = 0;
    int upgradedPackgesCount = 0;
    int currentPackagesCount = 0;
    fprintf(outputFile, "Pacman Packages Report\n");
    fprintf(outputFile, "----------------------\n");

    for(int i = 0; i < SIZE; i++){

        if(hashArray[i] == NULL){
            continue;
        }

        char packageStatus[1000];
        strcpy(packageStatus, hashArray[i]->status);

        if( strcmp(packageStatus, "installed") == 0 ){
            ++installedPackagesCount;

        } else if( strcmp(packageStatus, "removed") == 0 ){
            ++removedPackagesCount;

        } else if( strcmp(packageStatus, "upgraded") == 0 ){
            ++upgradedPackgesCount;
        }
    }
    currentPackagesCount = installedPackagesCount - removedPackagesCount;

    fprintf(outputFile,"- Installed packages  : [%d]", installedPackagesCount);
    fprintf(outputFile,"\n- Removed packages    : [%d]", removedPackagesCount);
    fprintf(outputFile,"\n- Upgraded packages   : [%d]", upgradedPackgesCount);
    fprintf(outputFile,"\n- Current packages    : [%d]\n", currentPackagesCount);
    fprintf(outputFile,"----------------------\n");


}    
    
void getGeneralStats(int sciptletCount, int pacmanCount,FILE *inputFile , FILE *outputFile){
    int alpmCount = 0;

    fprintf(outputFile, "General Stats\n");
    fprintf(outputFile, "----------------------\n");

    for(int i = 0; i < SIZE; i++){

        if(hashArray[i] == NULL){
            continue;
        }

        char manager[1000];
        strcpy(manager, hashArray[i]->manager);

        if( strcmp(manager, "[ALPM]") == 0 ){
            ++alpmCount;
        }
    }

    fprintf(outputFile, "- Oldest package               : ");
    getOldest(outputFile);
    fprintf(outputFile, "- Newest package               : ");
    getNewest(outputFile);
    fprintf(outputFile, "- Package with no upgrades     : ");
    for(int i = 0; i < SIZE; i++){
        if(hashArray[i] == NULL){
            continue;
        }


        if( hashArray[i]->numberUpdtaes == 0 ){
            fprintf(outputFile, "%s, ", hashArray[i]->name);
        }
    }

    fprintf(outputFile, "\n");
    fprintf(outputFile, "- [ALPM-SCRIPTTLET] type count : %d",  sciptletCount);
    fprintf(outputFile, "\n- [ALPM] count                 : %d", alpmCount);
    fprintf(outputFile, "\n- [PACMAN] count               : %d", pacmanCount);
    fprintf(outputFile, "\n----------------------");

}

void printListOfPackages(FILE *outputFile){

    fprintf(outputFile, "\nList of packages\n");
    fprintf(outputFile, "----------------------\n");
    for(int i = 0; i < SIZE; i++){

        if(hashArray[i] == NULL){
            continue;
        }

        fprintf(outputFile, "- Package Name           : %s\n", hashArray[i]->name );
        fprintf(outputFile, "    - Install date       : %s\n", hashArray[i]->InstallDate );
        fprintf(outputFile, "    - Last update date   : %s\n", hashArray[i]->lastUpdate );
        fprintf(outputFile, "    - How many updates   : %d\n", hashArray[i]->numberUpdtaes );
        fprintf(outputFile, "    - Removal date       : %s\n\n", hashArray[i]->removalDate );
    }
}

void getOldest(FILE *outputFile){
    char oldest[1000];

    int i = 0;
    while(hashArray[i] == NULL){
        ++i;
    }
    strcpy(oldest, hashArray[i]->name );
    fprintf(outputFile, "%s\n", oldest);
}

void getNewest(FILE *outputFile){
    char newest[1000];

    int i = SIZE;
    while(hashArray[i] == NULL){
        --i;
    }
    strcpy(newest, hashArray[i]->name );
    fprintf(outputFile, "%s\n", newest);
}

bool checkExtension(char line[1000]){
    char sentence[1000];
    strcpy(sentence, line);
    char* word = strtok(sentence, ".");

    while (word != NULL) {
        if(strcmp(word,"txt") == 0) {
            return true;
            printf("Encontrado: %s\n", word);
        }
        word = strtok(NULL, "."); 
    }
    strcpy(sentence, line);

    return false;
}



// ------ Metodos de la HT --------

void insert( struct Package *myPackage) {

   struct Package *item = (struct Package*) malloc(sizeof(struct Package));
    strcpy(item->name, myPackage->name );
    strcpy(item->InstallDate, myPackage->InstallDate);
    strcpy(item->lastUpdate, myPackage->lastUpdate);
    strcpy(item->status, myPackage->status);
    strcpy(item->manager, myPackage->manager);
    item->numberUpdtaes = myPackage->numberUpdtaes;
    strcpy(item->removalDate, myPackage->removalDate);

   //get the hash 
   int hashIndex = string_hashCode(myPackage->name);

   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL) {
      ++hashIndex;
      hashIndex %= SIZE;
   }
	
   hashArray[hashIndex] = item;
}

struct Package *search(char key[1000]) {

   int hashIndex = string_hashCode(key);
	
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
      if( strcmp(hashArray[hashIndex]->name, key) == 0){
          return hashArray[hashIndex]; 
      }	
      ++hashIndex;
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}
