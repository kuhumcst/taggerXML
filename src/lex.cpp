#include "useful.h"
#include "lex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *append_with_space(char *w1,char *w2)
    {
    char *result;
    
    result = 
        (char *)malloc((strlen(w1) + strlen(w2)+2) * sizeof(char)); 
    INCREMENT
    strcpy(result,w1);
    strcat(result," ");
    strcat(result,w2);
    return(result); 
    }

char *append_with_char(char *w1,char *w2,char w3)
    {
    char *result;
    
    result = 
        (char *)malloc((strlen(w1) + strlen(w2)+2) * sizeof(char)); 
    INCREMENT
    sprintf(result,"%s%c%s",w1,w3,w2);
    return(result);
    }
  
int numspaces(char *buf)
    {
    int tot;
    size_t count;
    tot = 0;
    for (count = 0; count < strlen(buf); ++count) 
        {
        if (buf[count]==' ')
            ++tot;
        }
    return(tot); 
    }

char **perl_split(char *buf)
    {
    char **return_buf;
    int cntr = 0;
    char *temp,*temp2;
    
    temp2 = (char *)malloc(sizeof(char)*(1+strlen(buf)));
    INCREMENT
    while(*buf == ' ' || *buf == '\t') 
        ++buf;
    strcpy(temp2,buf);
    return_buf = (char **) malloc(sizeof(char *) * ((numspaces(temp2)+1) + 2));
    INCREMENT
    return_buf[cntr++] = (char *)strtok(temp2," \t");
    while ((temp = (char *)strtok(NULL," \t")) != NULL) 
        {
        if (temp != NULL) 
            {
            return_buf[cntr] = temp;
            ++cntr;
            }
        }
    return_buf[cntr] = NULL;
    return(return_buf); 
    }



char **perl_split_independent(char *buf)
    {
    char **return_buf;
    int cntr = 0;
    char *temp;
    
    while(*buf == ' ' || *buf == '\t') 
        ++buf;
    return_buf = (char **) malloc(sizeof(char *) * (numspaces(buf)+3));
    INCREMENT
    return_buf[cntr++] = (char *)mystrdup((char *)strtok(buf," \t"));
    while ((temp = (char *)strtok(NULL,"\t ")) != NULL) 
        {
        return_buf[cntr] =(char *)mystrdup(temp);
        ++cntr;
        }
    return_buf[cntr] = NULL;
    return(return_buf); 
    }



#if 0
char **perl_split_on_char(char *buf,char achar)
    {
    char **return_buf;
    int cntr = 0;
    char *temp,temp2[2],*temp3;
    
    temp3 = (char *)malloc(sizeof(char)*(1+strlen(buf)));
    INCREMENT
    temp2[0] = achar; temp2[1] = '\0';
    return_buf = (char **) malloc(sizeof(char *) * ((numchars(temp3,achar)+1) + 2));
    INCREMENT
    return_buf[cntr++] = (char *)strtok(temp3,temp2);
    while ((temp = (char *)strtok(NULL,temp2)) != NULL) 
        {
        if (temp != NULL) 
            {
            return_buf[cntr] = temp;
            ++cntr;
            }
        }
    return_buf[cntr] = NULL;
    return(return_buf); 
    }

#endif


char **perl_split_on_nothing(char *buf)
    {
    char **return_buf;
    size_t cntr;
    char *temp2;  
    
    temp2 = (char *)malloc(sizeof(char)*(1+strlen(buf)));
    INCREMENT
    strcpy(temp2,buf);
    
    return_buf = (char **) malloc(sizeof(char *) * (strlen(buf)+1));
    INCREMENT
    for (cntr = 0; cntr < strlen(buf); ++cntr) 
        {
        return_buf[cntr] = (char *)malloc(sizeof(char)*2);
        INCREMENT
        return_buf[cntr][0] = temp2[cntr];
        return_buf[cntr][1] = '\0'; 
        }
    return_buf[cntr] = NULL;
    return(return_buf); 
    }

char *perl_explode(char *buf)
    {
    char *return_buf;
    size_t cntr;

    return_buf = (char *) malloc(sizeof(char) * ((strlen(buf)*2)+1));
    INCREMENT
    for (cntr = 0; (cntr/2) < strlen(buf); cntr+=2) 
        {
        return_buf[cntr] = buf[cntr/2];
        return_buf[cntr+1] = ' '; 
        }
    return_buf[cntr-1] = '\0';
    return(return_buf); 
    }

//char **perl_free(ptr) 
void perl_free(char **ptr) // Bart 20001218
    {
    free(ptr[0]); 
    DECREMENT
    }





char *return_tag(char *theword)
    {
    char *tempword;
    tempword = (char *)strchr(theword,'/');
    if (tempword != NULL)  
        return (tempword+1); 
    else
        return(NULL); 
    }


char *before_tag(char *theword)
    {
    int count = 0;
    
    while (theword[count] != '\0' &&
        theword[count] != '/')
        count++;
    if (theword[count] == '/')
        theword[count] = '\0';
    return(theword); 
    }



