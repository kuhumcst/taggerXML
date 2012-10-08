#ifndef WORD_H
#define WORD_H

#include "defines.h"
#include <string.h>
#if STREAM
# include <iostream>
# ifndef __BORLANDC__
using namespace std;
# endif
#else
# include <stdio.h>
#endif


extern void (*print)(FILE * fp,const char * m_word);

class taggedWord;
class Word;
typedef void (Word::*trav0)();
typedef void (taggedWord::*trav0T)();
typedef void (Word::*trav0C)()const;
typedef void (Word::*trav)(void *);

typedef int (Word::*cmp_f)(const Word *) const;
typedef int (taggedWord::*cmp_ft)(const taggedWord *) const;
class text;


class Word
    {
#ifdef COUNTOBJECTS
    public:
        static int COUNT;
#endif
    protected:
        bool hasAddedItselfToBaseForm:1;
        bool FoundInDict:1;
        bool owns:1;
    protected:
    public:
        char * m_word;
    protected:
        int cnt;
    public:
        void inc(){++cnt;}
        const char * itsWord()const{return m_word;}
        static cmp_f cmp;
        Word(const char * word)
            : 
                hasAddedItselfToBaseForm(false),FoundInDict(false),owns(true)
                ,cnt(1)
            {
            this->m_word = new char[strlen(word) + 1];
            strcpy(this->m_word,word);
#ifdef COUNTOBJECTS
            ++COUNT;
#endif
            }
        Word(const Word & w)
            : 
                hasAddedItselfToBaseForm(w.hasAddedItselfToBaseForm),
                FoundInDict(w.FoundInDict),
                owns(false),
                m_word(w.m_word),
                cnt(w.cnt)
            {
#ifdef COUNTOBJECTS
            ++COUNT;
#endif
            }
        virtual ~Word()
            {
            if(owns)
                {
                delete m_word;
                }
#ifdef COUNTOBJECTS
            --COUNT;
#endif
            }
    };

#endif
