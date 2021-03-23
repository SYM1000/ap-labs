

int mystrlen(char *str){
    int count = 0;
    char * p;
    
    for (p = str; *p != '\0'; p++) {
        count++;
    }

    return count;
}

char *mystradd(char *origin, char *addition){
    char *newStr = origin;
    char *end = addition;
    int i = 0;

    while(*newStr != '\0'){
        newStr++;
    }   

    while(*end != '\0'){
        *newStr++ = *end++;
    }

    *newStr = '\0';

    return origin;
    

}

int mystrfind(char *origin, char *substr){
    int pos = 0;
    int len = mystrlen(substr);
    int originPos = 0;
    int subPos = 0;

    while(origin[originPos] != '\0') {
        pos = originPos;


        while(origin[originPos] != '\0' && origin[originPos]==substr[subPos] ){
            subPos++;
            originPos++;
        }

        if(subPos == len){
            return pos;

        }else{
            subPos = 0;
            originPos++;   
            
        }

        
        
    }


    return -1;
}




