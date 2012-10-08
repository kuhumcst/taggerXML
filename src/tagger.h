#ifndef TAGGER_H
#define TAGGER_H

#include "option.h"
#include "defines.h"
#if STREAM
# include <iostream>
# ifndef __BORLANDC__
using namespace std;
# endif
#else
#include <stdio.h>
#endif
#include <malloc.h>

/*
#if defined __MSDOS__ || defined _WIN32			//if PC version

#define START_PROG "start-state-tagger.exe"
#define END_PROG "final-state-tagger.exe"

#else  

#define START_PROG "./start-state-tagger" 
#define END_PROG "./final-state-tagger"

#endif
*/

class tagger
    {
    private:
    public:
        tagger();
        bool init(
                const char * Lexicon,
                const char * Bigrams,
                const char * Lexicalrulefile,
                const char * Contextualrulefile,
                char *intermed,
                char * wdlistname,
                bool START_ONLY_FLAG,
                bool FINAL_ONLY_FLAG
                );
        ~tagger();
#if STREAM
        bool analyse(istream & CORPUS,ostream * fintermed,ostream & fpout,optionStruct * Options);
#else
        bool analyse(FILE * CORPUS,FILE * fintermed,FILE * fpout,optionStruct * Options);
#endif
    };


#endif
