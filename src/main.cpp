/*
Version 1: Brill's tagger
Version 2: CST's adaptation (standard C, removal of language dependent code, introduction of xoptions, handling of capitals in headlines
Version 3: handling of XML (Previously called version 1, because Brill had not defined a version.)
*/
#define CSTTAGGERVERSION "3.5"
#define CSTTAGGERDATE "2016.05.03"
#define CSTTAGGERCOPYRIGHT "MIT, University of Pennsylvania and Center for Sprogteknologi"

#include "useful.h" // dupl
#include "tagger.h"
#include "option.h"
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
#else
#include <stdio.h>
#include <stdlib.h>
#endif

#if defined __MSDOS__ || defined _WIN32			//if PC version
#else
#include <unistd.h>
#endif
#define TIMING 0
#if TIMING
#include <time.h>
#endif
#include "argopt.h"

int main(int argc, char **argv)
    {
    if(argc == 1)
        {
        printf("\n");
        printf("CSTTAGGER version " CSTTAGGERVERSION " (" CSTTAGGERDATE ")\n");
        printf("Copyright (C) " CSTTAGGERCOPYRIGHT "\n");
// GNU >> 
        printf("CSTTAGGER comes with ABSOLUTELY NO WARRANTY; for details use option -W.\n");
        printf("This is free software, and you are welcome to redistribute it under\n");
        printf("certain conditions; use option -r for details.\n");
        printf("\n\n");
// << GNU
        printf("Use option -h for usage.\n");
        return 0;
        }

    optionStruct Option;
    OptReturnTp optResult = Option.readArgs(argc,argv);

    if(optResult == Error)
        return 1;

    if(optResult == Leave)
        { // option -r, -w, -? or -h
        return 0;
        }

    int i = oldoptind;
#if defined __MSDOS__ || defined _WIN32			//if PC version
#else
   // --i;
#endif

    if(Option.Output != NULL)
        {
        if(freopen(Option.Output,"w",stdout) == NULL)
            {
            fprintf(stderr,"Cannot reassign stdout to %s\n",Option.Output);
            return 1;
            }
        }

    for(;i < argc;++i) 
        {
        if(argv[i][0] == '-')
            {
            fprintf(stderr,"Options must come before arguments\n");
            return 1;
            }
        if(argv[i][0] == '>')
            {
        // Bart 20090824
        // Background: a bug in VC2008 causes "> myoutput" to be interpreted as two extra arguments by the debugger.
            continue;
            }
        if(!Option.Lexicon)
            Option.Lexicon = dupl(argv[i]);
        else if(!Option.Corpus)
            Option.Corpus = dupl(argv[i]);
        else if(!Option.Bigrams)
            Option.Bigrams = dupl(argv[i]);
        else if(!Option.Lexicalrulefile)
            Option.Lexicalrulefile = dupl(argv[i]);
        else if(!Option.START_ONLY_FLAG && !Option.Contextualrulefile)
            Option.Contextualrulefile = dupl(argv[i]);
        else
#if _DEBUG
        // Bart 20090824
        // Background: a bug in VC2008 causes "> myoutput" to be interpreted as two extra arguments by the debugger.
        // Workaround: Just add myoutput as an extra argument during debugging. (without >)
        //freopen(argv[6+temp],"w",stdout);
            {
            if(Option.Output == NULL)
                freopen(argv[i],"w",stdout);
            }
#else
            {
            fprintf(stderr,"TOO MANY ARGUMENTS\n");
            return 1;
            }
#endif
        }

    if(!Option.Lexicon)
        {
        fprintf(stderr,"Lexicon not defined\n");
        return 1;
        }
    else if(!Option.Corpus)
        {
        fprintf(stderr,"Corpus not defined\n");
        return 1;
        }
    else if(!Option.Bigrams)
        {
        fprintf(stderr,"Bigrams not defined\n");
        return 1;
        }
    else if(!Option.Lexicalrulefile)
        {
        fprintf(stderr,"Lexical rule file not defined\n");
        return 1;
        }
    tagger theTagger;

    if(!theTagger.init  (Option.Lexicon
                        ,Option.Bigrams
                        ,Option.Lexicalrulefile
                        ,Option.Contextualrulefile
//                        ,Option.intermed           // m
                        ,Option.wdlistname         // w
                        //,splitnum           // s
                        ,Option.START_ONLY_FLAG    // S
                        ,Option.FINAL_ONLY_FLAG    // F
                        )
      )
        return 1;

/*
#if STREAM
    ofstream * fintermed = NULL;
#else
    FILE * fintermed = NULL;
#endif
    if(Option.intermed)
        {
#if STREAM
        fintermed = new ofstream(Option.intermed);
        if(!fintermed->is_open())
#else
        fintermed = fopen(Option.intermed,"w");
        if(!fintermed)
#endif
            {
            fprintf(stderr,"Cannot open intermediary file \"%s\" for writing\n",Option.intermed);
            exit(1);
            }
        }
*/

#if STREAM
    ifstream corpus(Option.Corpus);
    corpus.unsetf(ios::skipws); // do not skip white space
    theTagger.analyse(corpus,/*fintermed,*/cout,&Option);
#else
    FILE * out = stdout;//fopen("output.txt","wb");
    FILE * corpus = fopen(Option.Corpus,"r");
    if(!corpus)
        {
        fprintf(stderr,"Cannot open file %s for reading\n",Option.Corpus);
        return 1;
        }
    theTagger.analyse(corpus,/*fintermed,*/out,&Option);
#endif
    
#if STREAM
    /*
    if(fintermed)
        {
        delete fintermed;
        }*/
#else
    if(out != stdout)
        fclose(out);
    fclose(corpus);
    /*
    if(fintermed)
        {
        fclose(fintermed);
        }*/
#endif

#if TIMING
    printf("%ld/%ld",(long int)clock() - (long int)time0,(long int)CLOCKS_PER_SEC);
#endif
    return 0;
    }


