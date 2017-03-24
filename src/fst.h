#ifndef FST_H
#define FST_H

#include "XMLtext.h"
#include "registry.h"
#include "defines.h"
#if STREAM
# include <iostream>
# ifndef __BORLANDC__
using namespace std;
# endif
#else
#include <stdio.h>
#endif

struct optionStruct;

int final_state_tagger(  const char * Contextualrulefile
#if RESTRICT_MOVE
#if WITHSEENTAGGING
                       , Registry SEENTAGGING
#endif
                       , Registry WORDS
#endif
                       , corpus * Corpus
                       , optionStruct * Options
#if STREAM
                       , ostream & fpout
#else
                       , FILE * fpout
#endif
                       );

#endif
