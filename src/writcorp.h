#ifndef WRITECORPUS_H
#define WRITECORPUS_H

#include "defines.h"

#if STREAM
# include <iostream>
# ifndef __BORLANDC__
using namespace std;
# endif
#else
#include <stdio.h>
#endif


#if STREAM
void writeCorpus(int corpus_max_index,char ** word_corpus_array,char ** tag_corpus_array,ostream & fpout);
#else
void writeCorpus(int corpus_max_index,char ** word_corpus_array,char ** tag_corpus_array,FILE * fpout);
#endif
#endif
