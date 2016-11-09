/* mygets.c : safe replacement for gets(char *) */

#define _CRT_SECURE_NO_WARNINGS

#include "mygets.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>


#define BUFINC 5000 /* Bart 20030415 */
#define INCREMENT
#define DECREMENT

static char * theLine = NULL;
static int theLinelen = 0;


char * mygets() /* Bart 20030415 */
    {
    char * readbuf = NULL;
    char * ret;
    size_t readbuflen = 0;
    size_t L;
    int hasread = 0;
    if(!theLine)
        {
        theLine = (char *)malloc(BUFINC);
        INCREMENT
        theLinelen = BUFINC;
        }
    readbuf = theLine;
    readbuflen = theLinelen;
    while((ret = fgets(readbuf,readbuflen,stdin)) != NULL && (L = strlen(readbuf)) == readbuflen - 1) 
        {
        char * newline;
        newline = (char *)malloc(theLinelen + BUFINC);
        INCREMENT
        strcpy(newline,theLine);
        free(theLine);
        DECREMENT
        theLine = newline;
        readbuf = theLine + theLinelen - 1;
        readbuflen = BUFINC + 1;
        theLinelen += BUFINC;
        hasread = 1;
        }
    if(ret || hasread)
        {
        int L = strlen(theLine) - 1;
        if(theLine[L] == '\n')
            theLine[L] = '\0';
        return theLine;
        }
    else
        return NULL;
    }

