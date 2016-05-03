#include "utf8func.h"
#include "useful.h"
#include <string.h>
#include <stdlib.h> // Bart 20001218
#include <stdio.h> // Bart 20030224
#include <ctype.h> // Bart 20030225

/* Set 'product' to the name of the environment variable that contains (part
   of the) path to the options file, not including the file name itself.
   If NULL, a file named 'xoptions' is assumed to be in the current working
   directory. 
   Default value: NULL*/
char * product = NULL;
/* Set 'relative_path_to_xoptions' to the path to the options file, including
   the file name itself and relative to the value stored in the <product>
   environment variable. 
   If NULL, a file named 'xoptions' is assumed to be in the directory
   specified in the <product> environment varaible. (or in the CWD, if 
   product == NULL.
   Default value: NULL */
char * relative_path_to_xoptions = NULL;
char * xoptions = NULL;

int allocated = 0;

int strcmp_nocase(const char * a, const char * b)
    {
    int ret = strcmp(a,b);
    if(ret && !isUpperUTF8(a) && isUpperUTF8(b))
        {
        ret = strcmp(a,allToLowerUTF8(b));
        }
    return ret;
    }

char * dupl(const char * s)
    {
    char * d = new char[strlen(s) + 1];
    strcpy(d,s);
    return d;
    }

char *mystrdup(const char *thestr) 
    {
    INCREMENT
    return strcpy((char *)malloc(strlen(thestr)+1),thestr);
    }

int not_just_blank(char *thestr)
/* make sure not just processing a no-character line */
    { 
    char *thestr2;
    thestr2 = thestr;
    while(*thestr2 != '\0') 
        {
        if (*thestr2 != ' ' && *thestr2 != '\t' && *thestr2 != '\n')
            {
            return(1); 
            }
        ++thestr2;
        }
    return(0);
    }

int num_words(char *thestr)
    {
    int count,returncount;
    
    returncount=0;
    count=0;
    while (  thestr[count] != '\0' 
          && (  thestr[count] == ' ' 
             || thestr[count] == '\t'
             )
          ) 
        ++count;
    while (thestr[count++] != '\0') 
        {
        if (thestr[count-1] == ' ' || thestr[count-1] == '\t') 
            {
            ++returncount;
            while (thestr[count] == ' ' || thestr[count] == '\t')
                ++count;
            if (thestr[count] == '\0') 
                --returncount;
            }
        }
    return(returncount);
    }

static char * getBaseDir()
    {
    if(xoptions)
        return xoptions; // Bart 20060321. New option -x <optionfile>

    // loff
    static char basedir[1028];
    char * basedir_ptr;
    // fprintf(stderr,"Initialising, key: %s\n",key);
    if (product && (basedir_ptr = getenv(product /*"QA_DANISH"*/)) != NULL )
        {
        strcpy(basedir,basedir_ptr);
        if(relative_path_to_xoptions)
            strcat(basedir,relative_path_to_xoptions /*"/preprocess/xoptions"*/);
        else
            strcat(basedir,"xoptions");
        // fprintf(stderr,"Initialising using getenv %s\n",basedir);
        }
    else
        strcpy(basedir,"xoptions");
    // loff end
    
    //fprintf(stderr,"Initialising DAQPreprocess, using: %s\n",basedir);
    
    //loff FILE * fp = fopen("xoptions","r");
    return basedir;
    }

long option(const char * key) 
    {
    FILE * fp = fopen(getBaseDir(),"r");
    if(fp)
        {
        char line[256];
        while(fgets(line,255,fp))
            {
            if(!strncmp(key,line,strlen(key)))
                {
                char * eqpos = strchr(line,'=');
                if(eqpos)
                    {
                    fclose(fp);
                    return strtol(eqpos+1,NULL,0);
                    }
                else
                    {
                    break;;
                    }
                }
            }
        fclose(fp);
        }
    return -1;
    }

char * coption(const char * key) 
    {
    //loff FILE * fp = fopen("xoptions","r");
    FILE * fp = fopen(getBaseDir(),"r");
    if(fp)
        {
        static /*Bart 20030224*/ char line[256];
        while(fgets(line,255,fp))
            {
            if(!strncmp(key,line,strlen(key)))
                {
                char * startp = strchr(line,'=');
                if(startp)
                    {
                    char * endp;
                    while(isspace((unsigned char)*++startp))
                        ;
                    for(endp = startp;*endp && !isspace((unsigned char)*endp);++endp)
                        ;
                    *endp = '\0';
                    /*if(fp)
                    {
                    fprintf(fp,"%s = \"%s\"\n",key,startp);
                    fclose(fp);
                    }*/
                    fclose(fp);
                    return startp;
                    }
                else
                    break;
                }
            }
        fclose(fp);
        }
    return NULL;
    }

