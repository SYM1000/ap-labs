#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct Date {
    int   day;
    char  month[100];
    int   year;
};

/* month_day function's prototype*/
void month_day(int year, int yearDay, struct Date *myDate);

int main(int args, char**argv) {

    struct Date myDate;
    
    if(args == 3){
        //DayYear input validation
        bool isLeap = atoi(argv[1])%4 == 0 && ( !(atoi(argv[1])%100==0) || atoi(argv[1])%400 == 0);
        if( (atoi(argv[2]) > 365 && !isLeap) || (atoi(argv[2]) > 366 && isLeap) ){
            printf("Invalid dayYear Input\n");
            return 0;
        }

        //Inputs are valids
        month_day( atoi(argv[1]), atoi(argv[2]), &myDate);
        printf("%s %d, %d\n", myDate.month, myDate.day, myDate.year);

        return 0;
    }else{
        printf("Invalid Input\n");
        return 0;
    }

}

void month_day(int year, int yearDay, struct Date *myDate) {

        //define some variables
        bool isLeap = year%4 == 0 && ( !(year%100==0) || year%400 == 0);
        static char* monthsNames[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct", "Nov","Dec"};
        static int monthsDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        int count = 0;
        int i = 0;

        if(isLeap){
            monthsDays[1] = 29;
        }
        
        while(count < yearDay){
            count = count + monthsDays[i];
            ++i;
        }

        if(count == yearDay){
                --i;
                myDate->day = monthsDays[i];
                strcpy(myDate->month, monthsNames[i]);
                myDate->year = year;
                
        }else{
                --i;
                count = count - monthsDays[i];
                myDate->day = yearDay - count;
                strcpy(myDate->month, monthsNames[i]);
                myDate->year = year;
        }

    }
