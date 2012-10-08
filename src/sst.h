#ifndef SST_H
#define SST_H

#include "XMLtext.h"
#include "registry.h"
#include "defines.h"
#include <assert.h>
#include <string.h>

class strng
    {
    private:
        char * Key;
        char * Val;
    public:
        strng(const char * key,const char * val)
            {
            assert(key != 0);
            Key = new char[strlen(key)+1];
            strcpy(Key,key);
            if(val)
                {
                Val = new char[strlen(val)+1];
                strcpy(Val,val);
                }
            else
                Val = 0;
            }
        ~strng()
            {
            delete [] Key;
            delete [] Val;
            }
        const char * key() const
            {
            return Key;
            }
        char * val() const
            {
            return Val;
            }
        void setVal(const char * val)
            {
            if(val)
                {
                Val = new char[strlen(val)+1];
                strcpy(Val,val);
                }
            else
                delete [] Val;
            }
    };

struct optionStruct;

int start_state_tagger
        (
        Registry lexicon_hash,
        corpus * Corpus,
        Registry bigram_hash,
        Darray rule_array,
        Registry wordlist_hash,
        const char * Wordlist,
        optionStruct * Options
        );

#endif
