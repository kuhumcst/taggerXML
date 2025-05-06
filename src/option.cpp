
#include "option.h"
#include "useful.h"
#include "argopt.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* xoptions:
f ConvertToLowerCaseIfFirstWord = 1
a ConvertToLowerCaseIfMostWordsAreCapitalized = 1
s ShowIfLowercaseConversionHelped = 0
//l Language = danish  (One of the LC_CTYPES accepted by setlocale(), e.g.
// "danish", "dutch", "english", "french", "german", "italian", "spanish")
n Noun = N_INDEF_SING
p Proper = EGEN
v Verbose = 1
*/


static char opts[] = "@:?aB:C:d:D:fFhi:L:m:n:o:p:sSvw:x:X:" /* GNU: */ "Wr";
// Notice: with the current implementation of getopt, -X must 
// be the last option
// OR
// be followed by another option
// OR
// have an argument itself, be it '-' or one of the suboptions
static char *** Ppoptions = NULL;
static char ** Poptions = NULL;
static int optionSets = 0;

/*static bool boolean(const char * locoptarg)
    {
    return locoptarg ? *locoptarg == '-' ? false : true : true;
    }*/

optionStruct::optionStruct():
         Lexicon(0)
        ,Corpus(0)
        ,Bigrams(0)
        ,Lexicalrulefile(0)
        ,Contextualrulefile(0)
        ,wdlistname(0)
        //,intermed(0)
        ,START_ONLY_FLAG(false)
        ,FINAL_ONLY_FLAG(false)
        ,xtra(0)
        ,ConvertToLowerCaseIfFirstWord(false) // -f
        ,ConvertToLowerCaseIfMostWordsAreCapitalized(false)// -a
        ,ShowIfLowercaseConversionHelped(false)// -s
        ,Noun(0) // -n 
        ,Proper(0)// -p
        ,Verbose(false)// -v
        ,XML(false)
        ,ancestor(0)
        ,segment(0)
        ,element(0)
        ,wordAttribute(0)
        ,PreTagAttribute(0)
        ,POSAttribute(0)
        ,Output(0)
        {
        }

    optionStruct::~optionStruct()
        {
        delete [] Lexicon;
        delete [] Corpus;
        delete [] Bigrams;
        delete [] Lexicalrulefile;
        delete [] Contextualrulefile;
        delete [] wdlistname;
        //delete [] intermed;
        delete [] xtra;
        delete [] Noun; // -n 
        delete [] Proper;// -p
        delete [] ancestor;
        delete [] segment;
        delete [] element;
        delete [] wordAttribute;
        delete [] PreTagAttribute;
        delete [] POSAttribute;
        delete [] Output;
        }

    OptReturnTp optionStruct::doSwitch(int c,char * locoptarg,char * progname)
        {
        switch (c)
            {
            case '@':
                readOptsFromFile(locoptarg,progname);
                break;
            case 'h':
            case '?':
                printf("usage:\n");
                printf("%s [options] [LEXICON] [CORPUS-TO-TAG] [BIGRAMS] [LEXICALRULEFILE] [CONTEXTUALRULEFILE]\n",progname);
                printf("options:\n");
                printf("    -@<optionsfile>\n");
                printf("    -h   help\n");
                printf("    -?   help\n");
                printf("    -D<LEXICON>\n");
                printf("    -i<CORPUS-TO-TAG>\n");
                printf("    -B<BIGRAMS>\n");
                printf("    -L<LEXICALRULEFILE>\n");
                printf("    -C<CONTEXTUALRULEFILE>\n");
                printf("    -w<WORDLIST>\n");
                printf("    -m<INTERMEDFILE>\n");
                printf("    -S   start state tagger only\n");
                printf("    -F   final state tagger only\n");
                printf("    -o<out> output (optional, otherwise stdout)\n");
                printf("    -r   About redistribution (GNU)\n");
                printf("    -W   About warranty (GNU)\n");
                printf("    -x<path> path to file with extra options (deprecated)\n");
                printf("    -x- do not use xoptions file at all\n");
                printf("        (Not specifyiing -x makes the tagger look for a file 'xoptions' in the CWD.)\n");
                printf("    -f ConvertToLowerCaseIfFirstWord (default off)\n");
                printf("    -a ConvertToLowerCaseIfMostWordsAreCapitalized (default off)\n");
                printf("    -s ShowIfLowercaseConversionHelped (default off)\n");
                //printf("    -l Language (default danish)  (One of the LC_CTYPES accepted by setlocale(), e.g. \"danish\", \"dutch\", \"english\", \"french\", \"german\", \"italian\", \"spanish\")\n");
                printf("    -n<class> Noun (default NN)\n");
                printf("    -p<class> Proper (default NNP)\n");
                printf("    -v Verbose (default off)\n");
                printf("============================\n");
                printf("    -X- Not XML input. XML tags will be treated as text and POS-tagged. (default)\n");
                printf("    -X+ XML input. Leave XML elements unchanged. POS as suffix behind word, separated by slash.\n");
                printf("    The next options do not allow space between option letters and argument!\n");
                printf("    -Xa<ancestor>  Only analyse elements with specified ancestor. e.g -Xap\n");
                printf("    -Xs<delimiter> Segment (sentence) delimiter. Can be empty tag. e.g -Xsbr or -Xss\n");
                printf("    -Xe<element>  Only analyse specified element. e.g -Xew\n");
                printf("    -Xw<word>  Words are to be found in attribute. e.g -Xwword\n");
                printf("    -Xt<pretag>  Words' pre-tagging to be found in attribute. e.g -Xtprepos\n");
                printf("    -Xp<POS>  Destination of POS is the specified attribute. e.g -Xppos\n");
                return Leave;
            case 'f':
                ConvertToLowerCaseIfFirstWord = true;//boolean(locoptarg);
                break;
            case 'a':
                ConvertToLowerCaseIfMostWordsAreCapitalized = true;//boolean(locoptarg);
                break;
            case 's':
                ShowIfLowercaseConversionHelped = true;//boolean(locoptarg);
                break;
            case 'n':
                Noun = dupl(locoptarg);//(default NN)\n");
                break;
            case 'p':
                Proper = dupl(locoptarg);// (default NNP)\n");
                break;
            case 'v':
                Verbose = true;//boolean(locoptarg);
                break;
            case 'D':
                //LEXICON
                Lexicon = dupl(locoptarg);
                break;
            case 'i':
                //CORPUS-TO-TAG
                Corpus = dupl(locoptarg);
                break;
            case 'o':
                Output = dupl(locoptarg);
                break;
            case 'B':
                //BIGRAMS
                Bigrams = dupl(locoptarg);
                break;
            case 'L':
                //LEXICALRULEFILE
                Lexicalrulefile = dupl(locoptarg);
                break;
            case 'C':
                //CONTEXTUALRULEFILE
                Contextualrulefile = dupl(locoptarg);
                break;
            case 'd':
            case 'w':
                //WORDLIST
                wdlistname = dupl(locoptarg);
                break;
                /*
            case 'm':
                //INTERMEDFILE
                intermed = dupl(locoptarg);
                break;
                */
            case 'S':
                //start state tagger only
                START_ONLY_FLAG = true;
                break;
            case 'F':
                //final state tagger only
                FINAL_ONLY_FLAG = true;
                break;
            case 'x':
                //path to file with extra options
                xoptions = dupl(locoptarg);
                break;
            case 'X':
                if(locoptarg)
                    {
                    if(*locoptarg == '-')
                        {
                        XML = false;
                        }
                    else
                        {
                        XML = true;
                        switch(*locoptarg)
                            {
                            case 'a':
                                ancestor = dupl(locoptarg+1);
                                break;
                            case 's':
                                segment = dupl(locoptarg+1);
                                break;
                            case 'e':
                                element = dupl(locoptarg+1);
                                break;
                            case 'w':
                                wordAttribute = dupl(locoptarg+1);
                                break;
                            case 't':
                                PreTagAttribute = dupl(locoptarg+1);
                                break;
                            case 'p':
                                POSAttribute = dupl(locoptarg+1);
                                break;
                            }
                        }
                    }
                else
                    XML = true;
                break;

            case 'r':
                printf("12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n");
                printf("WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n");
                printf("REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n");
                printf("INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n");
                printf("OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n");
                printf("TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n");
                printf("YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n");
                printf("PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n");
                printf("POSSIBILITY OF SUCH DAMAGES.\n");
                return Leave;
            case 'W':
                printf("11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n");
                printf("FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n");
                printf("OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n");
                printf("PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n");
                printf("OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n");
                printf("MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n");
                printf("TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n");
                printf("PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n");
                printf("REPAIR OR CORRECTION.\n");
                return Leave;
            }
        return GoOn;
        }



    OptReturnTp optionStruct::readOptsFromFile(char * locoptarg,char * progname)
        {
        char ** poptions;
        char * options;
        FILE * fpopt = fopen(locoptarg,"r");
        OptReturnTp result = GoOn;
        if(fpopt)
            {
            char * p;
            char line[1000];
            int lineno = 0;
            int bufsize = 0;
            while(fgets(line,sizeof(line) - 1,fpopt))
                {
                lineno++;
                int off = strspn(line," \t\v\f");
                if(line[off] == ';')
                    continue; // comment line
                if(line[off] == '-')
                    {
                    off++;
                    if(line[off])
                        {
                        char * locoptarg2 = line + off + 1;
                        int off2 = strspn(locoptarg2," \t\v\f");
                        if(!locoptarg2[off2])
                            locoptarg2 = NULL;
                        else
                            locoptarg2 += off2;
                        if(locoptarg2)
                            {
                            for(p = locoptarg2 + strlen(locoptarg2) - 1;p >= locoptarg2;--p)
                                {
                                if(!isspace((unsigned char)*p))
                                    break;
                                *p = '\0';
                                }
                            bool string = false;
                            if(*locoptarg2 == '\'' || *locoptarg2 == '"')
                                {

                                // -x 'jhgfjhagj asdfj\' hsdjfk' ; dfaasdhfg
                                // -x 'jhgfjhagj asdfj\' hsdjfk' ; dfa ' asdhfg
                                // -x "jhgfjhagj \"asdfj hsdjfk" ; dfaasdhfg
                                // -x "jhgfjhagj \"asdfj hsdjfk" ; dfa " asdhfg
                                for(p = locoptarg2 + strlen(locoptarg2) - 1;p > locoptarg2;--p)
                                    {
                                    if(*p == *locoptarg2)
                                        {
                                        string = true;
                                        for(char * q = p + 1;*q;++q)
                                            {
                                            if(*q == ';')
                                                break;
                                            if(!isspace((unsigned char)*q))
                                                {
                                                string = false;
                                                }
                                            }
                                        if(string)
                                            {
                                            *p = '\0';
                                            ++locoptarg2;
                                            }
                                        break;
                                        }
                                    }
                                }
                            if(!*locoptarg2 && !string)
                                locoptarg2 = NULL;
                            }
                        if(locoptarg2)
                            {
                            bufsize += strlen(locoptarg2) + 1;
                            }
                        char * optpos = strchr(opts,line[off]);
                        if(optpos)
                            {
                            if(optpos[1] != ':')
                                {
                                if(locoptarg2)
                                    {
                                    printf("Option argument %s provided for option letter %c that doesn't use it on line %d in option file \"%s\"\n",locoptarg2,line[off],lineno,locoptarg);
                                    exit(1);
                                    }
                                }
                            }
                        }
                    else
                        {
                        printf("Missing option letter on line %d in option file \"%s\"\n",lineno,locoptarg);
                        exit(1);
                        }
                    }
                }
            rewind(fpopt);

            poptions = new char * [lineno];
            options = new char[bufsize];
            // update stacks that keep pointers to the allocated arrays.
            optionSets++;
            char *** tmpPpoptions = new char **[optionSets];
            char ** tmpPoptions = new char *[optionSets];
            int g;
            for(g = 0;g < optionSets - 1;++g)
                {
                tmpPpoptions[g] = Ppoptions[g];
                tmpPoptions[g] = Poptions[g];
                }
            tmpPpoptions[g] = poptions;
            tmpPoptions[g] = options;
            delete [] Ppoptions;
            Ppoptions = tmpPpoptions;
            delete [] Poptions;
            Poptions = tmpPoptions;

            lineno = 0;
            bufsize = 0;
            while(fgets(line,sizeof(line) - 1,fpopt))
                {
                poptions[lineno] = options+bufsize;
                int off = strspn(line," \t\v\f");
                if(line[off] == ';')
                    continue; // comment line
                if(line[off] == '-')
                    {
                    off++;
                    if(line[off])
                        {
                        char * locoptarg2 = line + off + 1;
                        int off2 = strspn(locoptarg2," \t\v\f");
                        if(!locoptarg2[off2])
                            locoptarg2 = NULL;
                        else
                            locoptarg2 += off2;
                        if(locoptarg2)
                            {
                            for(p = locoptarg2 + strlen(locoptarg2) - 1;p >= locoptarg2;--p)
                                {
                                if(!isspace((unsigned char)*p))
                                    break;
                                *p = '\0';
                                }
                            bool string = false;
                            if(*locoptarg2 == '\'' || *locoptarg2 == '"')
                                {

                                // -x 'jhgfjhagj asdfj\' hsdjfk' ; dfaasdhfg
                                // -x 'jhgfjhagj asdfj\' hsdjfk' ; dfa ' asdhfg
                                // -x "jhgfjhagj \"asdfj hsdjfk" ; dfaasdhfg
                                // -x "jhgfjhagj \"asdfj hsdjfk" ; dfa " asdhfg
                                for(p = locoptarg2 + strlen(locoptarg2) - 1;p > locoptarg2;--p)
                                    {
                                    if(*p == *locoptarg2)
                                        {
                                        string = true;
                                        for(char * q = p + 1;*q;++q)
                                            {
                                            if(*q == ';')
                                                break;
                                            if(!isspace((unsigned char)*q))
                                                {
                                                string = false;
                                                }
                                            }
                                        if(string)
                                            {
                                            *p = '\0';
                                            ++locoptarg2;
                                            }
                                        break;
                                        }
                                    }
                                }
                            if(!*locoptarg2 && /*Bart 20030905: allow empty string for e.g. -s option*/!string)
                                locoptarg2 = NULL;
                            }
                        if(locoptarg2)
                            {
                            strcpy(poptions[lineno],locoptarg2);
                            bufsize += strlen(locoptarg2) + 1;
                            }
                        else
                            {
                            strcpy(poptions[lineno],"");
                            }
                        /*else
                        locoptarg2 = "";
                        char * optpos = strchr(opts,line[off]);*/
                        OptReturnTp res = doSwitch(line[off],poptions[lineno],progname);
                        if(res > result)
                            result = res;
                        }
                    }
                lineno++;
                }
            fclose(fpopt);
            }
        else
            {
            fprintf(stderr,"Cannot open option file \"%s\" for reading\n",locoptarg);
            exit(1);
            }
        return result;
        }

    OptReturnTp optionStruct::readArgs(int argc, char * argv[])
        {
        int c;
        OptReturnTp result = GoOn;
        while((c = getopt(argc,argv, opts)) != -1)
            {
            OptReturnTp res = doSwitch(c,oldoptarg,argv[0]);
            if(res > result)
                result = res;
            }
        return result;
        }

