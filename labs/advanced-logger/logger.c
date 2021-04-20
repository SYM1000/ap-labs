#include <stdio.h>
#include "logger.h"
#include <stdarg.h>
#include <stdbool.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>

#define RESET 0
#define BRIGHT 1
#define DIM 2
#define UNDERLINE 3
#define BLINK 4
#define REVERSE 7
#define HIDDEN 8

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define	WHITE 7

bool isSysLog = false;

void resetColor(){
    printf("%c[0m",0x1B);
}

void setColor(int attr, int fg, int bg){
    char command[13];
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

int initLogger(char *logType) {
    setColor(RESET,GREEN,BLACK);
    isSysLog = false;
    printf("Initializing Logger on: %s\n", logType);

    if (strcmp(logType, "syslog") == 0){
        isSysLog = true;
    }

    resetColor();
    return 0;
}

void setLogType(int p,const char *format,va_list args){
    if(isSysLog == true){
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(p, format, args);
        closelog();
        
    } else{
        vprintf(format, args);
    }
}

int infof(const char *format, ...){
    setColor(RESET,BLUE,BLACK);
    va_list args;
    va_start(args,format);
    setLogType(LOG_INFO,format,args);
    va_end(args);
    resetColor();
    return 0;
}

int warnf(const char *format, ...){
    setColor(RESET,YELLOW,BLACK);
    va_list args;
    va_start(args,format);
    setLogType(LOG_WARNING,format,args);
    va_end(args);
    resetColor();
    return 0;
}

int errorf(const char *format, ...){
    va_list args;
    va_start(args,format);
    setColor(RESET,RED,BLACK);
    setLogType(LOG_ERR,format,args);
    va_end(args);
    resetColor();
    return 0;
}

int panicf(const char *format, ...){
    va_list args;
    va_start(args,format);
    setColor(RESET,MAGENTA,BLACK);
    setLogType(LOG_EMERG,format,args);
    va_end(args);
    resetColor();
    raise(SIGABRT);
    return 0;
}
