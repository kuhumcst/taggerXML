/* Copyright @ 1993 MIT and University of Pennsylvania */
/* Written by Eric Brill */
/*THIS SOFTWARE IS PROVIDED "AS IS", AND M.I.T. MAKES NO REPRESENTATIONS 
OR WARRANTIES, EXPRESS OR IMPLIED.  By way of example, but not 
limitation, M.I.T. MAKES NO REPRESENTATIONS OR WARRANTIES OF 
MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF 
THE LICENSED SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY 
PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.   */

/* Adapted by Bart Jongejan. (Conversion to Standard C and more.) */

#include "XMLtext.h"
#include "registry.h"
#include "useful.h"
#include "lex.h"
#include "sst.h"
#include "fst.h"
#include "staart.h"
#include "readcorpus.h"
#include "writcorp.h"
#include "tagger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if STREAM
# include <fstream>
# if defined __BORLANDC__
#  include <strstrea.h>
# else
#  ifdef __GNUG__
#   if __GNUG__ > 2
#    include <sstream>
#   else
#    include <strstream.h>
#   endif
#  else
#   include <sstream>
#  endif
# endif
# ifndef __BORLANDC__
using namespace std;
# endif
#endif


static const char * Lexicon;
static const char * Bigrams;
static const char * Lexicalrulefile;
static const char * Contextualrulefile;
static bool START_ONLY_FLAG;
static bool FINAL_ONLY_FLAG;
static char * wdlistname; 

static int corpussize;
static int linenums;
static int tagnums;
static Registry LEXICON_HASH;
static Registry BIGRAM_HASH;
static Registry WORDLIST_HASH;
static Darray RULE_ARRAY;

#if RESTRICT_MOVE
#if WITHSEENTAGGING
static Registry SEENTAGGING;
#endif
#if WITHWORDS
static Registry WORDS;
#endif
#endif

bool createRegistries
        (
#if RESTRICT_MOVE
#if WITHSEENTAGGING
        Registry * SEENTAGGING,
#endif
#if WITHWORDS
        Registry * WORDS,
#endif
#endif
        Registry * LEXICON_HASH,
        Registry * BIGRAM_HASH,
        Registry * WORDLIST_HASH,
        const char * Lexicon,
        const char * Lexicalrulefile,
        const char * Bigrams,
        const char * Wordlist,
#if !RESTRICT_MOVE
        int START_ONLY_FLAG,
#endif
        Darray * RULE_ARRAY
        )
    {
    int linenums = 0;
    int tagnums = 0;
    FILE * lexicon;
    FILE * rulefile;
    FILE * bigrams;
    FILE * wordlist;
    Registry lexicon_hash;
    Registry good_right_hash;
    Registry good_left_hash;
    Registry bigram_hash;
    Registry wordlist_hash;
    char line[5000];	/* input line buffer */
    Darray rule_array;
    char * linecopy;
    char **perl_split_ptr;
#if RESTRICT_MOVE
#if WITHSEENTAGGING
    char ** perl_split_ptr2;
    char * atempstr;
    char space[500];
    Registry seentagging;
#endif
    /* if in restrict-move mode (described above), */
    /* then we need to read in the lexicon file to see allowable */
    /* taggings for seen words*/
#if WITHWORDS
    char word[MAXWORDLEN];
    char tag[MAXTAGLEN];
    Registry words;
#endif
#else
    if (! START_ONLY_FLAG) 
#endif
        {
        lexicon = fopen(Lexicon,"r");
        if(!lexicon)
            {
            fprintf(stderr,"Cannot open lexicon \"%s\" for reading\n",Lexicon);
            exit(1);
            }
        while(fgets(line,sizeof(line),lexicon) != NULL) 
            {
            if (not_just_blank(line)) 
                {
                ++linenums;
                line[strlen(line) - 1] = '\0';
                tagnums += num_words(line);
                }
            }
        fclose(lexicon);
        }

#if RESTRICT_MOVE
#if WITHSEENTAGGING
    seentagging = *SEENTAGGING = Registry_create(Registry_strcmp,Registry_strhash);
    Registry_size_hint(seentagging,tagnums);
#endif

#if WITHWORDS
    words = *WORDS = Registry_create(Registry_strcmp,Registry_strhash);
    Registry_size_hint(words,linenums);
#endif
#endif

    lexicon_hash = *LEXICON_HASH = Registry_create(Registry_strcmp,Registry_strhash);
    Registry_size_hint(lexicon_hash,tagnums);

    lexicon = fopen(Lexicon, "r");
    if(!lexicon)
        {
        fprintf(stderr,"Cannot open lexicon \"%s\" for reading\n",Lexicon);
        exit(1);
        }
    while(fgets(line,sizeof(line),lexicon) != NULL)
    /*while(fgets(line,linelen,allowedmovefile) != NULL) */
        {/* Bart 20030415 */
        if (not_just_blank(line)) 
            {
            char *word2;
            char *tag2;
            line[strlen(line) - 1] = '\0';

    /* lexicon hash stores the most likely tag for all known words.
    we can have a separate wordlist and lexicon file because unsupervised
    learning    can add to wordlist, while not adding to lexicon.  For
    example, if a big    untagged corpus is about to be tagged, the wordlist
    can be extended to    include words in that corpus, while the lexicon
    remains static.    Lexicon is file of form: 
    word t1 t2 ... tn 
    where t1 is the most likely tag for the word, and t2...tn are alternate
    tags, in no particular order. */
    /* just need word and most likely tag from lexicon (first tag entry) */
#if RESTRICT_MOVE
#if WITHWORDS
            sscanf(line,"%s%s",word,tag);
            word2 = mystrdup(word);
            tag2 = mystrdup(tag);
            if(!Registry_add(lexicon_hash,(char *)word2,(char *)tag2))
                {
                free(word2);
                free(tag2);
                DECREMENT
                DECREMENT
                }

#if WITHSEENTAGGING
            perl_split_ptr = perl_split(line);
            perl_split_ptr2 = perl_split_ptr;
            ++perl_split_ptr;
            atempstr= mystrdup(*perl_split_ptr2);
            if(!Registry_add(words,atempstr,(char *)1))
                {
                free(atempstr);
                DECREMENT
                }
            while(*perl_split_ptr != NULL) 
                {
                sprintf(space,"%s %s",*perl_split_ptr2,*perl_split_ptr);
                atempstr = mystrdup(space);
                if(!Registry_add(seentagging,atempstr,(char *)1))
                    {
                    free(atempstr);
                    DECREMENT
                    }
                ++perl_split_ptr; 
                }
            free(*perl_split_ptr2);
            free(perl_split_ptr2);
            DECREMENT
            DECREMENT
#else
            char * space = strchr(line,' ');
            if(space)
                {
                *space = '\0';
                word2 = mystrdup(line);
                *space = ' ';
                tag2 = mystrdup(space+1);
                if(!Registry_add(words,word2,tag2))
                    {
                    free(word2);
                    free(tag2);
                    DECREMENT
                    DECREMENT
                    }
                }
#endif
#else
            char * space = strchr(line,' ');
            if(space)
                {
                *space = '\0';
                word2 = mystrdup(line);
                *space = ' ';
                tag2 = mystrdup(space+1);
                if(!Registry_add(lexicon_hash,word2,tag2))
                    {
                    free(word2);
                    free(tag2);
                    DECREMENT
                    DECREMENT
                    }
                }
#endif
#else
            char * space = strchr(line,' ');
            if(space)
                {
                *space = '\0';
                word2 = mystrdup(line);
                *space = ' ';
                tag2 = mystrdup(space+1);
                if(!Registry_add(lexicon_hash,word2,tag2))
                    {
                    free(word2);
                    free(tag2);
                    DECREMENT
                    DECREMENT
                    }
                }
#endif
            }
        }
    /* read in rule file */
    rule_array = *RULE_ARRAY = Darray_create();
    good_right_hash = Registry_create(Registry_strcmp,Registry_strhash);
    good_left_hash = Registry_create(Registry_strcmp,Registry_strhash);
    
    rulefile = fopen(Lexicalrulefile,"r");
    if(!rulefile)
        {
        fprintf(stderr,"Cannot open lexical rule file \"%s\" for reading\n",Lexicalrulefile);
        exit(1);
        }
    while(fgets(line,sizeof(line),rulefile) != NULL) 
        {
        if (not_just_blank(line))
            {
            char * tempruleptr;
            line[strlen(line) - 1] = '\0';
            perl_split_ptr = perl_split(line);
            Darray_addh(rule_array,perl_split_ptr);	   
            if (strcmp(perl_split_ptr[1],"goodright") == 0) 
                {
                tempruleptr = mystrdup(perl_split_ptr[0]);
                if(!Registry_add(good_right_hash,tempruleptr,(char *)1))
                    {
                    free(tempruleptr);
                    DECREMENT
                    }
                }
            else if (strcmp(perl_split_ptr[2],"fgoodright") == 0) 
                {
                tempruleptr = mystrdup(perl_split_ptr[1]);
                if(!Registry_add(good_right_hash,tempruleptr,(char *)1))
                    {
                    free(tempruleptr);
                    DECREMENT
                    } 
                }
            else if (strcmp(perl_split_ptr[1],"goodleft") == 0) 
                {
                tempruleptr = mystrdup(perl_split_ptr[0]);
                if(!Registry_add(good_left_hash,tempruleptr,(char *)1))
                    {
                    free(tempruleptr);
                    DECREMENT
                    } 
                }
            else if (strcmp(perl_split_ptr[2],"fgoodleft") == 0) 
                {
                tempruleptr = mystrdup(perl_split_ptr[1]);
                if(!Registry_add(good_left_hash,tempruleptr,(char *)1))
                    {
                    free(tempruleptr);
                    DECREMENT
                    }  
                }
            }
        }
    fclose(rulefile);

    /* read in bigram file */
    bigram_hash = *BIGRAM_HASH = Registry_create(Registry_strcmp,Registry_strhash);
    
    bigrams = fopen(Bigrams,"r");
    if(!bigrams)
        {
        fprintf(stderr,"Cannot open bigram file \"%s\" for reading\n",Bigrams);
        exit(1);
        }
    while(fgets(line,sizeof(line),bigrams) != NULL) 
        {
        if (strlen(line) > 1) 
            {
            char bigram1[MAXWORDLEN],bigram2[MAXWORDLEN];
            char bigram_space[MAXWORDLEN*2];
            line[strlen(line) - 1] = '\0';
            sscanf(line,"%s%s",bigram1,bigram2);
            if (Registry_get(good_right_hash,bigram1)) 
                {
                sprintf(bigram_space,"%s %s",bigram1,bigram2);
                linecopy = mystrdup(bigram_space);
                if(!Registry_add(bigram_hash,linecopy,(char *)1))
                    {
                    free(linecopy);
                    DECREMENT
                    } 
                }
            if (Registry_get(good_left_hash,bigram2)) 
                {
                sprintf(bigram_space,"%s %s",bigram1,bigram2);
                linecopy = mystrdup(bigram_space);
                if(!Registry_add(bigram_hash,linecopy,(char *)1))
                    {
                    free(linecopy);
                    DECREMENT
                    } 
                }
            }
        }
    fclose(bigrams);
    
    freeRegistry(good_right_hash);
    freeRegistry(good_left_hash);

    if (Wordlist) 
        {
        int numwordentries = 0;
        wordlist = fopen(Wordlist,"r");
        if(!wordlist)
            {
            fprintf(stderr,"Cannot open word lits \"%s\" for reading\n",Wordlist);
            exit(1);
            }
        while(fgets(line,sizeof(line),wordlist) != NULL) 
            {
            if (not_just_blank(line)) 
                ++numwordentries;
            }
        fclose(wordlist);
        wordlist_hash = *WORDLIST_HASH = Registry_create(Registry_strcmp,Registry_strhash);
        Registry_size_hint(wordlist_hash,numwordentries);
        wordlist = fopen(Wordlist,"r");
        if(!wordlist)
            {
            fprintf(stderr,"Cannot open word lits \"%s\" for reading\n",Wordlist);
            exit(1);
            }
        /* read in list of words */
        while(fgets(line,sizeof(line),wordlist) != NULL) 
            {
            if (not_just_blank(line)) 
                {
                char *word2;
                line[strlen(line) - 1] = '\0';
                word2 = mystrdup(line);
                if(!Registry_add(wordlist_hash,(char *)word2,(char *)1)) // Bart 20040203. Notice that the original source code has 'word', not 'word2' on this line. So that must be a bug!
                    {
                    free(word2);
                    DECREMENT
                    } 
                }
            }
        fclose(wordlist);
        }
    return true;
    }

static void deleteRegistries(
#if RESTRICT_MOVE
#if WITHSEENTAGGING
        Registry SEENTAGGING,
#endif
#if WITHWORDS
        Registry WORDS,
#endif
#endif
        Registry LEXICON_HASH,
        Registry BIGRAM_HASH,
        Registry WORDLIST_HASH
                             )
    {
#if RESTRICT_MOVE
#if WITHSEENTAGGING
    freeRegistry(SEENTAGGING);
#endif
#if WITHWORDS
    freeRegistry(WORDS);
#endif
#endif
    freeRegistry(LEXICON_HASH);
    freeRegistry(BIGRAM_HASH);
    freeRegistry(WORDLIST_HASH);
    }

tagger::tagger()
    {
    Lexicon = NULL;
    Bigrams = NULL;
    Lexicalrulefile = NULL;
    Contextualrulefile = NULL;
    corpussize = 0;
    linenums = 0;
    tagnums = 0;
    wdlistname = NULL; 
#if RESTRICT_MOVE
#if WITHSEENTAGGING
    SEENTAGGING = NULL;
#endif
#if WITHWORDS
    WORDS = NULL;
#endif
#endif
    LEXICON_HASH = NULL;
    BIGRAM_HASH = NULL;
    WORDLIST_HASH = NULL;
    RULE_ARRAY = NULL;
    }

bool tagger::init(
                const char * _Lexicon,
                const char * _Bigrams,
                const char * _Lexicalrulefile,
                const char * _Contextualrulefile,
                char * _wdlistname,
                bool _START_ONLY_FLAG,
                bool _FINAL_ONLY_FLAG
                )
    {
    Lexicon = _Lexicon;
    Bigrams = _Bigrams;
    Lexicalrulefile = _Lexicalrulefile;
    Contextualrulefile = _Contextualrulefile;
    START_ONLY_FLAG = _START_ONLY_FLAG;
    FINAL_ONLY_FLAG = _FINAL_ONLY_FLAG;
    wdlistname = _wdlistname;
    if (  (  START_ONLY_FLAG 
          && FINAL_ONLY_FLAG
          ) 
       || (  FINAL_ONLY_FLAG 
          && wdlistname
          ) 
       ) 
        {
        fprintf(stderr,"This set of options does not make sense.\n");
        return false;
        }
        

    return createRegistries
        (
#if RESTRICT_MOVE
#if WITHSEENTAGGING
        &SEENTAGGING, 
#endif
#if WITHWORDS
        &WORDS,
#endif
#endif
        &LEXICON_HASH,
        &BIGRAM_HASH,
        &WORDLIST_HASH,
        Lexicon,
        Lexicalrulefile,
        Bigrams,
        wdlistname,
#if !RESTRICT_MOVE
        START_ONLY_FLAG,
#endif
        &RULE_ARRAY
        );
    }


#if STREAM
bool tagger::analyse(istream & CORPUS,ostream & fpout,optionStruct * Options)
#else
bool tagger::analyse(FILE * CORPUS,FILE * fpout,optionStruct * Options)
#endif
    {
    text * Text = NULL;
    if(Options->XML)
        {
        Text = new XMLtext
           (CORPUS
           ,"$w\\s"//char * Iformat
           ,true//bool XML
           ,Options->ancestor//"p"//const char * ancestor // restrict POS-tagging to segments that fit in ancestor elements
           ,Options->segment
           ,Options->element//"w"//const char * element // analyse PCDATA inside elements
           ,Options->wordAttribute//NULL//const char * wordAttribute // if null, word is PCDATA
           ,Options->PreTagAttribute // Store POS in PreTagAttribute
           ,Options->POSAttribute//"lemma"//const char * POSAttribute // if null, Lemma is PCDATA
           );
        }
    else
        {
        Text = new text(CORPUS,FINAL_ONLY_FLAG);
        }

    hashmap::hash<strng> * tag_hash;
    tag_hash = new hashmap::hash<strng>(&strng::key,1000);
    if(!FINAL_ONLY_FLAG) 
        {
        corpussize = -1 + start_state_tagger
             (
             LEXICON_HASH,
             Text,
             BIGRAM_HASH,
             RULE_ARRAY,
             WORDLIST_HASH,
             wdlistname,
             Options,
             tag_hash
             );
        }
    if(!START_ONLY_FLAG)
         {
         final_state_tagger(Contextualrulefile
#if RESTRICT_MOVE
#if WITHSEENTAGGING
                           , SEENTAGGING
#endif
#if WITHWORDS
                           , WORDS
#else
                           , LEXICON_HASH
#endif
#endif
                           ,Text
                           ,Options
                           );
        }
    Text->printUnsorted(fpout);
    tag_hash->deleteThings();
    delete tag_hash;
    delete Text;
    return true;
    }

tagger::~tagger()
    {
    for (unsigned int i = 0;i < Darray_len(RULE_ARRAY);++i) 
        {
        char ** tempstr = (char **)Darray_get(RULE_ARRAY,i);
        free(*tempstr);
        free(tempstr);
        DECREMENT
        DECREMENT
        }
    Darray_destroy(RULE_ARRAY);
    deleteRegistries(
#if RESTRICT_MOVE
#if WITHSEENTAGGING
        SEENTAGGING,
#endif
#if WITHWORDS
        WORDS,
#endif
#endif
        LEXICON_HASH,BIGRAM_HASH,WORDLIST_HASH);
    }



