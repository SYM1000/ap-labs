
//Santiago Yeomans
#include <stdio.h>
#include <stdlib.h>

//#define   LOWER  0       /* lower limit of table */
//#define   UPPER  300     /* upper limit */
//#define   STEP   20      /* step size */

/* print Fahrenheit-Celsius table */

int main(int argc, char **argv)
{
    if(argc == 4){
        for(int i = atoi(argv[1]); i <= atoi(argv[3]); i = i + atoi(argv[2])){
            printf("Fahrenheit: %3d, Celcius: %6.1f\n", i , (5.0/9.0)*(i-32) );
        }
    }
    else if (argc == 2){
        printf("Fahrenheit: %3s, Celcius: %6.1f\n", argv[1], (5.0/9.0)*(atoi(argv[1])-32) );
    }
    else{
        printf("Error\n");
    } 

    return 0;
}
