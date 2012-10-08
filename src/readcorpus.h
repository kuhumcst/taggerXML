#ifndef READCORPUS_H
#define READCORPUS_H

#include "defines.h"
#if STREAM
# include <iostream>
# ifndef __BORLANDC__
using namespace std;
# endif
#else
#include <stdio.h>
#endif

int readcorpus (
#if STREAM
               istream & CORPUS,
#else
               FILE * CORPUS,
#endif
               char *** WORD_CORPUS_ARRAY,
               char *** TAG_CORPUS_ARRAY,
               int corpussize                
               );

#endif
