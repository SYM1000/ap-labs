#include <stdio.h>

#define IN   1   /* inside a word */
#define OUT  0   /* outside a word */

/* count lines, words, and characters in input */

int main()

{
    int i, state, pos;
    char c, word[100];
    //state = OUT;

while ((c = getchar()) != EOF) {    
	if (c == ' ' || c == '\n' || c == '\t') {
      //REVERSE
       for(int i =pos; i >= 1; i--){
        printf("%c", word[i]);
      }
       printf("%s", " ");

      pos = 0;
    }
  else {
    pos++;
	  word[pos] = c;
    pos++;
	}
    }

   printf("%s", " \n ");
    return 0;
}
