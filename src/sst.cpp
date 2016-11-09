/*  Copyright @ 1993 MIT and University of Pennsylvania*/
/* Written by Eric Brill */
/*THIS SOFTWARE IS PROVIDED "AS IS", AND M.I.T. MAKES NO REPRESENTATIONS 
OR WARRANTIES, EXPRESS OR IMPLIED.  By way of example, but not 
limitation, M.I.T. MAKES NO REPRESENTATIONS OR WARRANTIES OF 
MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF 
THE LICENSED SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY 
PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.   */

/* Adapted by Bart Jongejan. (Conversion to Standard C and more.) */

#include "substring.h"
//#include "hashmap.h"
#include "utf8func.h"
#include "darray.h"
#include "lex.h"
#include "memory.h"
#include "useful.h"
#include "sst.h"
#include "staart.h"
#include "defines.h"
#include "option.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>





int CheckLineForCapitalizedWordsOrNonAlphabeticStuff(corpus * Corpus)
    {
    int tot = 0;
    int low = 0;
    for(int i = Corpus->startOfLine;i <= Corpus->endOfLine;++i)
        {
        ++tot;
        if(!isUpperUTF8(Corpus->Token[i].getWord()))
            ++low;
        }
    return 2*low < tot; /* return 1 if not heading (less than half of all words begin with lower case.)*/
    }

int start_state_tagger
        (
        Registry lexicon_hash,
        corpus * Corpus,
        Registry bigram_hash,
        Darray rule_array,
        Registry wordlist_hash,
        const char * Wordlist,
        optionStruct * Options,
        hashmap::hash<strng> * tag_hash
        )
    {
    int corpus_array_index = 0;
    hashmap::hash<strng> * ntot_hash; // for words (types) that are not in the lexicon.
//    hashmap::hash<strng> * tag_hash;
    strng ** tag_array;
    char *tempstr,*tempstr2;
    char **therule,**therule2;
    char noun[20] = {'\0'},proper[20] = {'\0'};
    int rulesize,tempcount;
    unsigned int count,count2,count3;
    char
        tempstr_space[MAXWORDLEN+MAXAFFIXLEN],bigram_space[MAXWORDLEN*2];
    int EXTRAWDS= 0;
    long ConvertToLowerCaseIfFirstWord = option("ConvertToLowerCaseIfFirstWord");
    if(ConvertToLowerCaseIfFirstWord == -1)
        {
        ConvertToLowerCaseIfFirstWord = Options->ConvertToLowerCaseIfFirstWord ? 1 : 0;
        }

    long ConvertToLowerCaseIfMostWordsAreCapitalized = option("ConvertToLowerCaseIfMostWordsAreCapitalized");
    if(ConvertToLowerCaseIfMostWordsAreCapitalized == -1)
        {
        ConvertToLowerCaseIfMostWordsAreCapitalized = Options->ConvertToLowerCaseIfMostWordsAreCapitalized ? 1 : 0;
        }
    long Verbose = option("Verbose");
    if(Verbose == -1)
        {
        Verbose = Options->Verbose ? 1 : 0;
        }
    char * tmp;
    tmp = coption("Noun");
    if(tmp)
        {
        strncpy(noun,tmp,sizeof(noun) - 1);
        noun[sizeof(noun)  - 1] = '\0';
        }
    else if(Options->Noun)
        {
        strncpy(noun,Options->Noun,sizeof(noun) - 1);
        }

    tmp = coption("Proper");
    if(tmp)
        {
        strncpy(proper,tmp,sizeof(proper) - 1);
        proper[sizeof(proper)  - 1] = '\0';
        }
    else if(Options->Proper)
        {
        strncpy(proper,Options->Proper,sizeof(noun) - 1);
        }
    
    
    /***********************************************************************/
    
    
        /* Wordlist_hash contains a list of words.  This is used 
        for tagging unknown words in the "add prefix/suffix" and
    "delete prefix/suffix" rules.  This contains words not in LEXICON. */
    
    if (Wordlist) 
        {
        EXTRAWDS=1;
        }
    
    /*********************************************************/
    /* Read in Corpus to be tagged.  Actually, just record word list, */
    /* since each word will get the same tag, regardless of context. */
    ntot_hash = new hashmap::hash<strng>(&strng::key,1000);

    Corpus->rewind();
    
    INCREMENT
    INCREMENT

    
    while(Corpus->getline()) 
        {
            {
            int startOfLine = Bool_TRUE;
            int heading = Bool_FALSE;
            if(ConvertToLowerCaseIfMostWordsAreCapitalized)
                {
                heading = CheckLineForCapitalizedWordsOrNonAlphabeticStuff(Corpus);
                }
            const char * wrd;
            while ((wrd = Corpus->getWord()) != NULL) 
                { 
                if(  Registry_get(lexicon_hash,wrd) == NULL
                  && (  (  heading 
                        && (Registry_get(lexicon_hash,allToLowerUTF8(wrd)) == NULL)
                        )
                     || !startOfLine
                     || ConvertToLowerCaseIfFirstWord != 1
                     || Registry_get(lexicon_hash,allToLowerUTF8(wrd)) == NULL
                     )
                  ) 
                    {
                    void * bucket;
                    if(!ntot_hash->find(wrd,bucket))
                        {
                        ntot_hash->insert(new strng(wrd,NULL),bucket);
                        }
                    }
                startOfLine = Bool_FALSE;
                }
            substring::reset();
            }
        }
    Corpus->rewind();
    
    if(Verbose)
        {
        fprintf(stderr,"START STATE TAGGER:: CORPUS READ\n");  
        }
    
    
    size_t length;
    tag_array = ntot_hash->convertToList(length);
    delete ntot_hash;


    /********** START STATE ALGORITHM
    YOU CAN USE OR EDIT ONE OF THE TWO START STATE ALGORITHMS BELOW, 
# OR REPLACE THEM WITH YOUR OWN ************************/
    
    if(!*noun)
        {
        strcpy(noun,"NN");
        }
    if(!*proper)
        {
        strcpy(proper,"NNP");
        }
    
        /* UNCOMMENT THIS AND COMMENT OUT START STATE 2 IF ALL UNKNOWN WORDS
        SHOULD INITIALLY BE ASSUMED TO BE TAGGED WITH "NN".
    YOU CAN ALSO CHANGE "NN" TO A DIFFERENT TAG IF APPROPRIATE. */
    
    /*** START STATE 1 ***/
    /*   for (count= 0; count < Darray_len(tag_array_val);++count) 
    Darray_set(tag_array_val,count,noun); */
    
    /* THIS START STATE ALGORITHM INITIALLY TAGS ALL UNKNOWN WORDS WITH TAG 
    "NN" (singular common noun) UNLESS THEY BEGIN WITH A CAPITAL LETTER, 
    IN WHICH CASE THEY ARE TAGGED WITH "NNP" (singular proper noun)
    YOU CAN CHANGE "NNP" and "NN" TO DIFFERENT TAGS IF APPROPRIATE.*/
    
    /*** START STATE 2 ***/
    for (count= 0; count < length/*Darray_len(tag_array_val)*/;++count) 
        {
        if (isUpperUTF8(tag_array[count]->key()))
            {
            tag_array[count]->setVal(proper); 
            }
        else
            {
            tag_array[count]->setVal(noun); 
            }
        }
    
    
    
    /******************* END START STATE ALGORITHM ****************/

    // Analyse the Out Of Vocabulary words with the lexical rules.

    unsigned int maxcount = Darray_len(rule_array);
    size_t notokens = length;
    for (count= 0;count < maxcount;++count) 
        {
        therule = (char **)Darray_get(rule_array,count);
        if(Verbose)
            fprintf(stderr,"s");
            /* we don't worry about freeing "rule" space, as this is a small fraction
            of total memory used */
        therule2 = &therule[1];
        rulesize= 0;
        char **perl_split_ptr;
        perl_split_ptr = therule;
        while(*(++perl_split_ptr) != NULL) 
            {
            ++rulesize;
            }
        
        if (strcmp(therule[1],"char") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) != 0) 
                    {
                    if(strpbrk(tag_array[count2]->key(), therule[0]) != NULL) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        else if (strcmp(therule2[1],"fchar") == 0) 
            { 
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[0]) == 0) 
                    {
                    if(strpbrk(tag_array[count2]->key(), therule2[0]) != NULL) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        else if (strcmp(therule[1],"deletepref") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) != 0) 
                    {
                    const char * tempstr = tag_array[count2]->key();
                    for (count3 = 0;(int)count3 < atoi(therule[2]);++count3) 
                        {
                        if (tempstr[count3] != therule[0][count3])
                            break;
                        }
                    if ((int)count3 == atoi(therule[2])) 
                        {
                        tempstr += atoi(therule[2]);
                        if (  Registry_get(lexicon_hash,(char *)tempstr) != NULL 
                           || (  EXTRAWDS 
                              && Registry_get(wordlist_hash,(char *)tempstr) != NULL
                              )
                           )
                            {
                            tag_array[count2]->setVal(therule[rulesize-1]);
                            }
                        }
                    }
                }
            }
        
        else if (strcmp(therule2[1],"fdeletepref") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[0]) == 0)
                    { 
                    const char * tempstr=tag_array[count2]->key();
                    for (count3 = 0;(int)count3 < atoi(therule2[2]);++count3) 
                        {
                        if (tempstr[count3] != therule2[0][count3])
                            {
                            break;
                            }
                        }
                    if ((int)count3 == atoi(therule2[2])) 
                        {
                        tempstr += atoi(therule2[2]);
                        if (  Registry_get(lexicon_hash,(char *)tempstr) != NULL 
                           || (  EXTRAWDS 
                              && Registry_get(wordlist_hash,(char *)tempstr) != NULL
                              )
                           )
                            {
                            tag_array[count2]->setVal(therule[rulesize-1]);
                            }
                        }
                    }
                }
            }
        
        
        else if (strcmp(therule[1],"haspref") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) != 0) 
                    {
                    const char * tempstr = tag_array[count2]->key();
                    for (count3 = 0;(int)count3 < atoi(therule[2]);++count3) 
                        {
                        if (tempstr[count3] != therule[0][count3])
                            {
                            break;
                            }
                        }
                    if ((int)count3 == atoi(therule[2])) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        else if (strcmp(therule2[1],"fhaspref") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[0]) == 0)
                    { 
                    const char * tempstr=tag_array[count2]->key();
                    for (count3 = 0;(int)count3 < atoi(therule2[2]);++count3) 
                        {
                        if (tempstr[count3] != therule2[0][count3])
                            {
                            break;
                            }
                        }
                    if ((int)count3 == atoi(therule2[2])) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        
        else if (strcmp(therule[1],"deletesuf") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) != 0) 
                    {
                    const char * tempstr = tag_array[count2]->key();
                    tempcount=strlen(tempstr) - atoi(therule[2]);
                    for (count3 = tempcount;count3 < strlen(tempstr); ++count3) 
                        {
                        if (tempstr[count3] != therule[0][count3-tempcount])
                            {
                            break;
                            }
                        }
                    if (count3 == strlen(tempstr)) 
                        {
                        tempstr2 = mystrdup(tempstr);
                        tempstr2[tempcount] = '\0';
                        if (  Registry_get(lexicon_hash,(char *)tempstr2) != NULL 
                           || (  EXTRAWDS 
                              && Registry_get(wordlist_hash,(char *)tempstr2) != NULL
                              )
                           ) 
                            {
                            tag_array[count2]->setVal(therule[rulesize-1]);
                            }
                        free(tempstr2);
                        DECREMENT
                        }
                    }
                }
            }
        
        else if (strcmp(therule2[1],"fdeletesuf") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[0]) == 0)
                    { 
                    const char * tempstr=tag_array[count2]->key();
                    tempcount=strlen(tempstr) - atoi(therule2[2]);
                    for (count3 = tempcount;count3 < strlen(tempstr); ++count3) 
                        {
                        if (tempstr[count3] != therule2[0][count3-tempcount])
                            {
                            break;
                            }
                        }
                    if (count3 == strlen(tempstr))
                        {
                        tempstr2 = mystrdup(tempstr);
                        tempstr2[tempcount] = '\0';
                        if (  Registry_get(lexicon_hash,(char *)tempstr2) != NULL
                           || (  EXTRAWDS 
                              && Registry_get(wordlist_hash,(char *)tempstr2) != NULL
                              )
                           ) 
                            {
                            tag_array[count2]->setVal(therule[rulesize-1]);
                            }
                        free(tempstr2);
                        DECREMENT
                        }
                    }
                }
            }
        else if (strcmp(therule[1],"hassuf") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) != 0) 
                    {
                    const char * tempstr = tag_array[count2]->key();
                    tempcount=strlen(tempstr) - atoi(therule[2]);
                    for (count3 = tempcount;count3 < strlen(tempstr); ++count3) 
                        {
                        if (tempstr[count3] != therule[0][count3-tempcount])
                            {
                            break;
                            }
                        }
                    if (count3 == strlen(tempstr)) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        else if (strcmp(therule2[1],"fhassuf") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[0]) == 0)
                    { 
                    const char * tempstr = tag_array[count2]->key();
                    tempcount = strlen(tempstr) - atoi(therule2[2]);
                    for (count3 = tempcount;count3 < strlen(tempstr); ++count3) 
                        {
                        if (tempstr[count3] != therule2[0][count3-tempcount])
                            {
                            break;
                            }
                        }
                    if (count3 == strlen(tempstr))
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        else if (strcmp(therule[1],"addpref") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) == 0)
                    {
                    sprintf(tempstr_space,"%s%s",therule[0],tag_array[count2]->key());
                    if (  Registry_get(lexicon_hash,(char *)tempstr_space) != NULL
                       || (  EXTRAWDS 
                          && Registry_get(wordlist_hash,(char *)tempstr_space) != NULL
                          )
                       ) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        else if (strcmp(therule2[1],"faddpref") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) == 0)
                    {
                    sprintf(tempstr_space,"%s%s",therule2[0],tag_array[count2]->key());
                    if (  Registry_get(lexicon_hash,(char *)tempstr_space) != NULL
                       || (  EXTRAWDS 
                          && Registry_get(wordlist_hash,(char *)tempstr_space) != NULL
                          )
                       ) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        
        else if (strcmp(therule[1],"addsuf") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) != 0) 
                    {
                    const char * A = tag_array[count2]->key();
                    char * B = therule[0];
                    sprintf(tempstr_space,"%s%s", A,B);
                    if (  Registry_get(lexicon_hash,(char *)tempstr_space) != NULL
                       || (  EXTRAWDS 
                          && Registry_get(wordlist_hash,(char *)tempstr_space) != NULL
                          )
                       )
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        
        else if (strcmp(therule2[1],"faddsuf") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[0]) == 0) 
                    {
                    sprintf(tempstr_space,"%s%s", tag_array[count2]->key(), therule2[0]);
                    if (  Registry_get(lexicon_hash,(char *)tempstr_space) != NULL
                       || (  EXTRAWDS 
                          && Registry_get(wordlist_hash,(char *)tempstr_space) != NULL
                          )
                       )
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        
        else if (strcmp(therule[1],"goodleft") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) != 0) 
                    {
                    sprintf(bigram_space,"%s %s", tag_array[count2]->key(),therule[0]);
                    if (Registry_get(bigram_hash,(char *)bigram_space) != NULL) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        else if (strcmp(therule2[1],"fgoodleft") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[0]) == 0) 
                    {
                    sprintf(bigram_space,"%s %s",tag_array[count2]->key(),therule2[0]);
                    if (Registry_get(bigram_hash,(char *)bigram_space) != NULL) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        else if (strcmp(therule[1],"goodright") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[rulesize-1]) != 0) 
                    {
                    sprintf(bigram_space,"%s %s",therule[0],tag_array[count2]->key());
                    if (Registry_get(bigram_hash,(char *)bigram_space) != NULL) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        else if (strcmp(therule2[1],"fgoodright") == 0) 
            {
            for (count2 = 0;count2 < notokens;++count2) 
                {
                if (strcmp(tag_array[count2]->val(),therule[0]) == 0) 
                    {
                    sprintf(bigram_space,"%s %s",therule2[0],tag_array[count2]->key());
                    if (Registry_get(bigram_hash,(char *)bigram_space) != NULL) 
                        {
                        tag_array[count2]->setVal(therule[rulesize-1]);
                        }
                    }
                }
            }
        
        
        }
    if(Verbose)
        {
        fprintf(stderr,"\n");
        }
    
    
    /* now go from array to hash table */
    
//    tag_hash = new hashmap::hash<strng>(&strng::key,1000);
    for (count= 0;count < length;++count) 
        {
        const char * name = tag_array[count]->key();
        char * val = tag_array[count]->val();
        void * bucket;
        if(!tag_hash->find(name,bucket))
            {
            tag_hash->insert(new strng(name, val),bucket);
            }
        }

    for (size_t i = 0; i < length; i++) delete tag_array[i];
    delete [] tag_array;

    while(Corpus->getline()) 
        {
        int startOfLine = Bool_TRUE;
        int heading = Bool_FALSE;
        if(ConvertToLowerCaseIfMostWordsAreCapitalized)
            {
            heading = CheckLineForCapitalizedWordsOrNonAlphabeticStuff(Corpus);
            }

        token * Tok;
        const char * wrd;
        for(int i = Corpus->startOfLine;i <= Corpus->endOfLine;++i)
            {
            Tok = Corpus->Token+i;
            wrd = Tok->getWord();
            if(Tok->PreTag)
                {
                Tok->Pos = Tok->PreTag; // assume tag from input
                }
            else if(  (tempstr = (char *)Registry_get(lexicon_hash,wrd))!= NULL
                   || ( (  heading 
                        || (  startOfLine
                           && ConvertToLowerCaseIfFirstWord == 1
                           )
                        )
                      && (tempstr = (char *)Registry_get(lexicon_hash,allToLowerUTF8(wrd))) != NULL
                      )
                   )
                {
                Tok->Pos = tempstr; // assume tag from lexicon
                }
            else 
                {
                void * bucket;
                strng * found = tag_hash->find(wrd,bucket);
                assert(found);
                Tok->Pos = found->val(); // assume tag from morphological analysis
                }
            startOfLine = Bool_FALSE;
            }
        substring::reset();
        }
    Corpus->rewind();
/*
    tag_hash->deleteThings();
    delete tag_hash;
*/
    return corpus_array_index;
    }


