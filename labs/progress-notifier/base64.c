#include <stdio.h>
#include "logger.h"
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>

/* Referencias:
* https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C
*/

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

unsigned long progress = 0, total = 1;
size_t curr;
size_t src = 0;
int encode;
void handleProgressSignal(int);
int error();
int handleFile(char *, char *, int * (void *, long , char *, long *));

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   for (curr= 0; curr < dataLength; curr += 3) 
   {
      n = ((uint32_t)data[curr]) << 16;
      
      if((curr+1) < dataLength)
         n += ((uint32_t)data[curr+1]) << 8;
      
      if((curr+2) < dataLength)
         n += data[curr+2];

      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
    
      if(resultIndex >= resultSize) return 1;
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;
      result[resultIndex++] = base64chars[n1];

      if((curr+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;
         result[resultIndex++] = base64chars[n2];
      }

      if((curr+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;
         result[resultIndex++] = base64chars[n3];
      }
   }

   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         if(resultIndex >= resultSize) return 1;
         result[resultIndex++] = '=';
      } 
   }
   if(resultIndex >= resultSize) return 1;
   result[resultIndex] = 0;
   return 0; // reach here if everything went well
}

int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;
    
    curr = 0;
    while (in < end) {
        curr++;
        unsigned char c = d[*in++];
        
        switch (c) {
        case WHITESPACE: continue;
        case INVALID:    return 1;
        case EQUALS:
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++;
            
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1; // if buffer overflow
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;
            }   
        }
    }
   
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1; // if buffer overflow
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1; // if buffer overflow
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len;
    return 0;
}


void mySignal(){
    int progress = (float)curr / src * 100;
    infof("%s is %d%% complete\n", encode ? "Encoding of the file" : "Decoding of the file", progress);
}


void startMySignals(){
    signal(SIGINT, mySignal);
    signal(SIGUSR1, mySignal);
}


int main(int argc, char **argv){
    if(argc != 3){
        errorf("ERROR: Parameters are not correct\n");
        return 0;
    }

    startMySignals();

    char *sourceFile, *oldBuffer, *newBuffer;
    FILE *orgFile, *targetgFile;

    encode = strcmp(argv[1], "--decode")==0?0:1;
    sourceFile = argv[2];

    if ((orgFile = fopen(sourceFile, "r")) == NULL){
        errorf("ERROR: Cannot open source file\n");
        return 0;
    }

    struct stat stats;
    stat(sourceFile, &stats);
    size_t dstLength = 4 * ((stats.st_size + 2) / 3.0) + 3;

    newBuffer = malloc(sizeof(char) * dstLength);
    oldBuffer = malloc(sizeof(char) * stats.st_size);

    int line;
    char buf[BUFSIZ];
    while((line = fread(buf, sizeof(char), BUFSIZ, orgFile)) > 0){
        size_t toCopy;
        if(BUFSIZ > stats.st_size - src){
            toCopy = stats.st_size - src;
        }else{
            toCopy = BUFSIZ;
        }
        strncpy(oldBuffer + src, buf, toCopy);
        src += line;
    }
    fclose(orgFile);

    int res= encode? base64encode(oldBuffer, src, newBuffer, dstLength) : 
                     base64decode(oldBuffer, src, (unsigned char *)newBuffer, &dstLength);
    
    if (res!=0){
        errorf("ERROR: on %s the file\n",encode? "encoding":"decoding");
        free(oldBuffer);
        free(newBuffer);
        return 1;
    }
    char * targetgFileName = strchr(sourceFile, '.');
    *targetgFileName=0;
    sprintf(targetgFileName, "%s-%s.txt", sourceFile, encode ? "encoded" : "decoded");
    
    if((targetgFile = fopen(targetgFileName, "w")) == NULL){
        errorf("ERROR: Cannot open result file\n");
        free(oldBuffer);
        free(newBuffer);
        return 1;
    }
    size_t writes, maxRead;
    size_t totalWrites = 0;
    size_t dstLen = strlen(newBuffer);
    
    while(totalWrites < dstLen){

        if(BUFSIZ > dstLen - totalWrites){
            maxRead = dstLen - totalWrites;
        }else{
            maxRead = BUFSIZ;
        }

        if((writes = fwrite(newBuffer + totalWrites, sizeof(char), maxRead, targetgFile)) != maxRead){
            errorf("ERROR: Something is wrong with the result file \n");
            free(oldBuffer);
            free(newBuffer);
            fclose(targetgFile);
            return 1;
        }

       totalWrites = totalWrites + writes;

    }

    fclose(targetgFile);
    free(oldBuffer);
    free(newBuffer);
    return 0;
}
