#ifndef OPTION_H
#define OPTION_H

#include "defines.h"

typedef enum {GoOn = 0,Leave = 1,Error = 2} OptReturnTp;

#if defined _WIN32
#define commandlineQuote "\""
#else
#define commandlineQuote "\'"
#endif

struct optionStruct
    {
    char * Lexicon;
    char * Corpus;
    char * Bigrams;
    char * Lexicalrulefile;
    char * Contextualrulefile;
    char * wdlistname;
    //char * intermed;
    bool START_ONLY_FLAG;
    bool FINAL_ONLY_FLAG;
    char * xtra; // default: xoptions file (deprecated)

    bool ConvertToLowerCaseIfFirstWord; // -f
    bool ConvertToLowerCaseIfMostWordsAreCapitalized;// -a
    bool ShowIfLowercaseConversionHelped;// -s
    char * Noun; // -n 
    char * Proper;// -p
    bool Verbose;// -v

    bool XML;
    char * ancestor;
    char * segment;
    char * element;
    char * wordAttribute;
    char * PreTagAttribute;
    char * POSAttribute;
    char * Output;
    optionStruct();
    ~optionStruct();
    OptReturnTp doSwitch(int c,char * locoptarg,char * progname);
    OptReturnTp readOptsFromFile(char * locoptarg,char * progname);
    OptReturnTp readArgs(int argc, char * argv[]);
    };
#endif
