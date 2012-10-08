#include "useful.h"
#include "lex.h"
#include "readcorpus.h"
//#include "staart.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int readcorpus (
#if STREAM
                istream & corpus,
#else
                FILE * corpus,
#endif
               char *** WORD_CORPUS_ARRAY,
               char *** TAG_CORPUS_ARRAY,
               int corpussize                
               )
    { 
    char line[5000];
//    FILE * corpus = NULL;
    char ** word_corpus_array;
    char ** tag_corpus_array;
    int corpus_array_index = 0;
    *WORD_CORPUS_ARRAY = word_corpus_array = (char **)malloc(sizeof(char *) * corpussize);
    *TAG_CORPUS_ARRAY = tag_corpus_array = (char **)malloc(sizeof(char *) * corpussize);
    INCREMENT
    INCREMENT
    
    /* read in corpus from stdin.  This corpus has already been tagged (output of */
    /* the start state tagger) */
    
    /*	while (gets(line) != NULL) {*/
//    corpus = fopen(Corpus,"r");


#if STREAM
    if(corpus)
        {
        while(!corpus.eof())
            {
            corpus.getline(line,sizeof(line));
            if(not_just_blank(line))
                {  
                char **split_ptr;
                char **split_ptr2;
                /*
                word_corpus_array[corpus_array_index] = staart;
                tag_corpus_array[corpus_array_index++] = staart;
                word_corpus_array[corpus_array_index] = staart;
                tag_corpus_array[corpus_array_index++] = staart;
                */
                if (line[strlen(line) - 1] == ' ')
                    {
                    line[strlen(line) - 1] = '\0';
                    }
                split_ptr = perl_split(line);
                split_ptr2 = split_ptr;
                while (*split_ptr != NULL) 
                    {
                    char * atempstrptr;
                    atempstrptr = strrchr(*split_ptr,'/');
                    if (atempstrptr == NULL) 
                        {
                        perror("Every word in the input to the final state tagger must be tagged\n");
                        return(0);
                        }
                    *atempstrptr = '\0';
                    ++atempstrptr;
                    word_corpus_array[corpus_array_index] =
                        mystrdup(*split_ptr);
                    tag_corpus_array[corpus_array_index++] = mystrdup(atempstrptr);
                    ++split_ptr;
                    }
                free(*split_ptr2);
                free(split_ptr2);
                DECREMENT
                    DECREMENT
                }
            }
        }
    corpus.clear();
    corpus.seekg(0, ios::beg);
#else
    if(corpus)
        {
        while(fgets(line,sizeof(line),corpus) != NULL) 
            {  /* Bart 20030415 */
            if(not_just_blank(line))
                {  
                char **split_ptr;
                char **split_ptr2;
                /*
                word_corpus_array[corpus_array_index] = staart;
                tag_corpus_array[corpus_array_index++] = staart;
                word_corpus_array[corpus_array_index] = staart;
                tag_corpus_array[corpus_array_index++] = staart;
                */
                if (line[strlen(line) - 1] == ' ')
                    {
                    line[strlen(line) - 1] = '\0';
                    }
                split_ptr = perl_split(line);
                split_ptr2 = split_ptr;
                while (*split_ptr != NULL) 
                    {
                    char * atempstrptr;
                    atempstrptr = strrchr(*split_ptr,'/');
                    if (atempstrptr == NULL) 
                        {
                        perror("Every word in the input to the final state tagger must be tagged\n");
                        return(0);
                        }
                    *atempstrptr = '\0';
                    ++atempstrptr;
                    word_corpus_array[corpus_array_index] =
                        mystrdup(*split_ptr);
                    tag_corpus_array[corpus_array_index++] = mystrdup(atempstrptr);
                    ++split_ptr;
                    }
                free(*split_ptr2);
                free(split_ptr2);
                DECREMENT
                DECREMENT
                }
            }
        }
    rewind(corpus);
#endif
    return corpus_array_index;
    }
