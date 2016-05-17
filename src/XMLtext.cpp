/*
taggerXML - Brill's PoS tagger ported to C++ - can handle xml

Copyright (C) 2012 Center for Sprogteknologi, University of Copenhagen

(Files in this project without this header are 
 Copyright @ 1993 MIT and University of Pennsylvania)

This file is part of taggerXML.

taggerXML is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

taggerXML is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with taggerXML; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "field.h"
#include "XMLtext.h"
#include "word.h"
#include "parsesgml.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>


static void printXML(
#if STREAM
                     ostream & fpo
#else
                     FILE * fpo
#endif
                     ,const char * s)
    {
#if STREAM
    while(*s)
        {
        switch(*s)
            {
            case '<':
                fpo << "&lt;";
                break;
            case '>':
                fpo << "&gt;";
                break;
            case '&':
                if(strchr(s+1,';'))
                    fpo << '&';
                else
                    fpo << "&amp;";
                break;
            case '\'':
                fpo << "&apos;";
                break;
            case '"':
                fpo << "&quot;";
                break;
            default:
                fpo << *s;
            }
        ++s;
        }
#else
    while(*s)
        {
        switch(*s)
            {
            case '<':
                fprintf(fpo,"&lt;");
                break;
            case '>':
                fprintf(fpo,"&gt;");
                break;
            case '&':
                if(strchr(s+1,';'))
                    fputc('&',fpo);
                else
                    fprintf(fpo,"&amp;");
                break;
            case '\'':
                fprintf(fpo,"&apos;");
                break;
            case '"':
                fprintf(fpo,"&quot;");
                break;
            default:
                fputc(*s,fpo);
            }
        ++s;
        }
#endif
    }


corpus::corpus():startOfLine(4532),endOfLine(0),numberOfTokens(0),wordno(0),Token(NULL),CurrToken(NULL)
    {
    static char STAART[] = "STAART";
    staart.setWordpos(STAART,STAART+strlen(STAART));
    staart.setPOSpos(STAART,STAART+strlen(STAART));
    staart.Pos = STAART;
    }

class crumb
    {
    private:
        char * e;
        int L;
        crumb * next;
    public:
        crumb(const char * s,int len,crumb * Crumbs):L(len),next(Crumbs)
            {
            e = new char[len+1];
            strncpy(e,s,len);
            e[len] = '\0';
            }
        ~crumb()
            {
            delete [] e;
            }
        const char * itsElement()
            {
            return e;
            }
        crumb * pop(const char * until,int len)
            {
            crumb * nxt = next;
            if(L == len && !strncmp(e,until,len))
                {
                delete this;
                return nxt;
                }
            else
                {
                delete this;
                return nxt ? nxt->pop(until,len) : NULL;
                }
            }
        void print()
            {
            if(next)
                next->print();
            printf("->%s",e);
            }
        bool is(const char * elm)
            {
            return !strcmp(e,elm);
            }
    };

bool XMLtext::analyseThis()
    {
    if(element)
        {
        return Crumbs && Crumbs->is(element);
        }
    else
        {
        if(ancestor)
            {
            return Crumbs != NULL;
            }
        else
            {
            return true;
            }
        }
    }


const char * XMLtext::convert(const char * s)
    {
    const char * p = strchr(s,'&');
    if(p && strchr(p+1,';'))
        {
        static char * ret[2] = {NULL,NULL};
        static size_t len[2] = {0,0};
        static int index = 0;
        index += 1;
        index %= 2;
        if(len[index] <= strlen(s))
            {
            delete ret[index];
            len[index] = strlen(s)+1;
            ret[index] = new char[len[index]];
            }
        char *p = ret[index];
        while(*s)
            {
            if(*s == '&')
                {
                ++s;
                if(!strncmp(s,"lt;",3))
                    {
                    *p++ = '<';
                    s += 3;
                    }
                else if(!strncmp(s,"gt;",3))
                    {
                    *p++ = '>';
                    s += 3;
                    }
                else if(!strncmp(s,"amp;",4))
                    {
                    *p++ = '&';
                    s += 4;
                    }
                else if(!strncmp(s,"apos;",5)) // Notice: XML standard, but not HTML
                    {
                    *p++ = '\'';
                    s += 5;
                    }
                else if(!strncmp(s,"quot;",5))
                    {
                    *p++ = '"';
                    s += 5;
                    }
                else
                    *p++ = '&';
                }
            else
                *p++ = *s++;
            }
        *p = '\0';
        return ret[index];
        }
    return s;
    }


void XMLtext::CallBackStartElementName()
    {
    startElement = ch;
    }

void XMLtext::CallBackEndElementName()
    {
    endElement = ch;
    if(ClosingTag)
        {
        if(Crumbs)
            {
            Crumbs = Crumbs->pop(startElement,endElement - startElement);
            if(!Crumbs)
                { // End of segment reached
                if(this->total > 0 && this->Token)
                    {
                    this->Token[total-1].lastOfLine = true;
                    this->Token[total].firstOfLine = true;
                    }
                }
            }
        ClosingTag = false;
        }
    else
        {
        if(  Crumbs
          || !this->ancestor
          || (  startElement + strlen(this->ancestor) == endElement 
             && !strncmp(startElement,this->ancestor,endElement - startElement)
             )
          )
            {
            Crumbs = new crumb(startElement,endElement - startElement,Crumbs);
            if(  segment 
              && this->total > 0 
              && this->Token 
              && !strncmp(startElement,segment,endElement - startElement)
              )
                {
                this->Token[total-1].lastOfLine = true;
                this->Token[total].firstOfLine = true;
                }
            }
        }
    }

void XMLtext::CallBackStartAttributeName()
    {
    if(analyseThis())
        {
        ClosingTag = false;
        startAttributeName = ch;
        }
    }

void XMLtext::CallBackEndAttributeNameCounting()
    {
    if(analyseThis())
        {
        endAttributeName = ch;
        if(  this->wordAttributeLen == endAttributeName - startAttributeName 
            && !strncmp(startAttributeName,this->wordAttribute,wordAttributeLen )
            )
            {
            ++total;
            }
        }
    }

void XMLtext::CallBackEndAttributeNameInserting()
    {
    if(analyseThis())
        {
        endAttributeName = ch;

        WordPosComing = false;
        PreTagPosComing = false;
        POSPosComing = false;


        if(  this->wordAttributeLen == endAttributeName - startAttributeName 
            && !strncmp(startAttributeName,this->wordAttribute,wordAttributeLen )
            )
            {
            WordPosComing = true;
            }
        else if(  this->POSAttributeLen == endAttributeName - startAttributeName 
            && !strncmp(startAttributeName,this->POSAttribute,POSAttributeLen)
            )
            {
            POSPosComing = true;
            }
        else if(  this->PreTagAttributeLen == endAttributeName - startAttributeName 
            && !strncmp(startAttributeName,this->PreTagAttribute,PreTagAttributeLen )
            )
            {
            PreTagPosComing = true;
            }
        }
    }

void XMLtext::CallBackStartValue()
    {
    if(analyseThis())
        {
        startValue = ch;
        }
    }

void XMLtext::CallBackEndValue()
    {
    if(analyseThis())
        {
        endValue = ch;            
        if(WordPosComing)
            {
            this->Token[total].setWord(startValue,endValue,this);
            }
        else if(PreTagPosComing)
            {
            this->Token[total].setPreTagpos(startValue,endValue);
            }
        else if(POSPosComing)
            {
            this->Token[total].setPOSpos(startValue,endValue);
            }
        }
    }

void XMLtext::CallBackNoMoreAttributes()
    {
    if(analyseThis())
        {
        token * Tok = CurrToken;
        if(Tok && Tok->WordGetStart() != NULL && Tok->WordGetEnd() != Tok->WordGetStart())
            { // Word as attribute
            char * EW = Tok->WordGetEnd();
            char savW = *EW;
            *EW = '\0';
            if(Tok->PreTagGetStart() != NULL)
                {
                char * EP = Tok->PreTagGetEnd();
                char savP = *EP;
                *EP = '\0';
                const char * pretag = convert(Tok->PreTagGetStart());
                Tok->Pos = Tok->PreTag = new char[strlen(pretag)+1];
                strcpy(Tok->PreTag,pretag);
                *EP = savP;
                }
            *EW = savW;
            CurrToken = NULL;
            }
        }
    }

void XMLtext::CallBackEndTag()
    {
    ClosingTag = true;
    }

void XMLtext::CallBackEmptyTag()
    {
    if(Crumbs)
        Crumbs = Crumbs->pop(startElement,endElement - startElement);
    }

class wordReader
    {
    private:
        field * format;
        field * nextfield;
        field * wordfield;
        unsigned long newlines;
        unsigned long lineno;
        const char * tag;
        int kar;
        int lastkar;
        XMLtext * Text;
        char kars[2];
    public:
        unsigned long getNewlines()
            {
            return newlines;
            }
        unsigned long getLineno()
            {
            return lineno;
            }
        void initWord()
            {
            nextfield = format;
            newlines = 0;
            format->reset();
            kars[1] = '\0';
            if(lastkar)
                {
                if(lastkar == '\n')
                    ++newlines;
                kars[0] = (char)lastkar;
                lastkar = 0;
                nextfield->read(kars,nextfield);
                }
            token * Tok = Text->getCurrentToken();
            if(Tok)
                {
                if(kars[0])
                    Tok->setWordpos(Text->ch-1,Text->ch-1);
                else
                    Tok->setWordpos(Text->ch,Text->ch);
                }
            }
        wordReader(field * format,field * wordfield,XMLtext * Text)
            :format(format)
            ,nextfield(format)
            ,wordfield(wordfield)
            ,newlines(0)
            ,lineno(0)
            ,tag(NULL)
            ,kar(EOF)
            ,lastkar(0)
            ,Text(Text)
            {
            initWord();
            assert(wordfield);
            if(lastkar)
                {
                assert(lastkar != EOF);
                if(lastkar == '\n')
                    ++newlines;
                kars[0] = (char)lastkar;
                lastkar = 0;
                nextfield->read(kars,nextfield);
                }
            }
#ifndef CONSTSTRCHR
        const 
#endif
        char * readChar(int kar)
            {
            if(!nextfield)
                {
                initWord();
                }
            if(kar == '\n')
                ++newlines;
            kars[0] = kar == EOF ? '\0' : (char)kar;
            char * plastkar = nextfield->read(kars,nextfield);
            if(kar == '\0')
                lastkar = '\0';
            else if(plastkar)
                lastkar = *plastkar;

            if(nextfield)
                return NULL;
            else
                {
                lineno += newlines;
                return wordfield->getString();
                }
            }
#ifndef CONSTSTRCHR
        const 
#endif
        char * count(int kar)
            {
#ifndef CONSTSTRCHR
            const 
#endif
            char * w = readChar(kar);
            if(Text->analyseThis())
                {
                if(w && *w)
                    {
                    Text->incTotal();
                    }
                }
            return w;
            }
#ifndef CONSTSTRCHR
        const 
#endif
        char * read(int kar)
            {
#ifndef CONSTSTRCHR
            const 
#endif
            char * w = readChar(kar);
            if(Text->analyseThis())
                {
                if(w && *w)
                    {
                    token * Tok = Text->getCurrentToken();
                    char * start = Tok->WordGetStart();
                    Tok->setWord(start,start+strlen(w),Text);
                    const char * pretag = Tok->PreTagGetStart();
                    if(pretag != NULL)
                        {
                        char * EP = Tok->PreTagGetEnd();
                        char savP = *EP;
                        *EP = '\0';
                        pretag = Text->convert(pretag);
                        Tok->Pos = Tok->PreTag = new char[strlen(pretag)+1];
                        strcpy(Tok->PreTag,pretag);
                        *EP = savP;
                        char temp[100];
                        sprintf(temp,"[%s]",Tok->Pos);
                        }
                    Text->CurrToken = NULL;
                    }
                }
            return w;
            }
    };

void XMLtext::printUnsorted(
#if STREAM
                            ostream & fpo
#else
                            FILE * fpo            
#endif
                            )
    {
    if(this->alltext)
        {
        unsigned int k;
        const char * o = this->alltext;
        for(k = 0;k < total;++k)
            {
            const char * start = Token[k].WordGetStart();
            const char * end = Token[k].WordGetEnd();
            const char * POSstart = Token[k].POSGetStart();
            const char * POSend = Token[k].POSGetEnd();
            char * p = strchr(Token[k].Pos,' ');
            if(p)
                *p = 0;
            if(POSstart == NULL)
                { // write word together with POS-tag
                if(start)
                    {
                    copy(fpo,o,end);
                    o = end;
                    }
                printXML(fpo,"/");
                printXML(fpo,Token[k].Pos);
                }
            else
                {
                copy(fpo,o,POSstart);
                o = POSend;
                printXML(fpo,Token[k].Pos);
                }
            if(p)
                *p = ' ';
            }
        o = copy(fpo,o,o+strlen(o));
        }
    }

token * XMLtext::getCurrentToken()
    {
    return Token ? Token + total : NULL;
    }

void CallBackStartElementName(void *arg)
    {
    ((XMLtext *)arg)->CallBackStartElementName();
    }

void CallBackEndElementName(void *arg)
    {
    ((XMLtext *)arg)->CallBackEndElementName();
    }

void CallBackStartAttributeName(void *arg)
    {
    ((XMLtext *)arg)->CallBackStartAttributeName();
    }

static void (XMLtext::*CallBackEndAttributeNames)();

void CallBackEndAttributeName(void *arg)
    {
    (((XMLtext *)arg)->*CallBackEndAttributeNames)();
    }

void CallBackStartValue(void *arg)
    {
    ((XMLtext *)arg)->CallBackStartValue();
    }

void CallBackEndValue(void *arg)
    {
    ((XMLtext *)arg)->CallBackEndValue();
    }

void CallBackNoMoreAttributes(void *arg)
    {
    ((XMLtext *)arg)->CallBackNoMoreAttributes();
    }

void CallBackEndTag(void * arg)
    {
    ((XMLtext *)arg)->CallBackEndTag();
    }

void CallBackEmptyTag(void * arg)
    {
    ((XMLtext *)arg)->CallBackEmptyTag();
    }

XMLtext::XMLtext(
#if STREAM
            istream & fpi
#else
            FILE * fpi
#endif
           ,const char * Iformat
           ,bool XML
           ,const char * ancestor // restrict POS-tagging to segments that fit in ancestor elements
           ,const char * segment // Optional segment delimiter. E.g. br: <br /> or s: <s> ... </s>
           ,const char * element // analyse PCDATA inside elements
           ,const char * wordAttribute // if null, word is PCDATA
           ,const char * PreTagAttribute // Fetch pretagging from PreTagAttribute
           ,const char * POSAttribute // Store POS in POSAttribute
           )
           :text()
           ,ancestor(ancestor)
           ,segment(segment)
           ,element(element)
           ,PreTagAttribute(PreTagAttribute)
           ,POSAttribute(POSAttribute)
           ,Crumbs(NULL)
           ,ClosingTag(false)
           ,WordPosComing(false)
           ,PreTagPosComing(false)
           ,POSPosComing(false)
           ,wordAttribute(wordAttribute)
    {
    wordAttributeLen = wordAttribute ? strlen(wordAttribute) : 0;
    PreTagAttributeLen = PreTagAttribute ? strlen(PreTagAttribute) : 0;
    POSAttributeLen = POSAttribute ? strlen(POSAttribute) : 0;
#ifdef COUNTOBJECTS
    ++COUNT;
#endif

    field * wordfield;
    field * format = 0;
    
    if(XML && Iformat)
        {
#if STREAM
        fpi.seekg(0,ios::end);
        long filesize = fpi.tellg();
        fpi.seekg(0,ios::beg);
        if(filesize > 0)
            {
            alltext = new char[filesize+1];
            char * p = alltext;
            char * e = alltext + filesize;
            while(p < e)
                {
                char kar;
                fpi >> kar;
                *p++ = kar;
                }
            *p = '\0';
            fpi.seekg(0,ios::beg);
            }
#else
        int kar;
        fseek(fpi,0,SEEK_END);
        long filesize = ftell(fpi);
        ::rewind(fpi);
        if(filesize > 0)
            {
            alltext = new char[filesize+1];
            char * p = alltext;
            while((kar = getc(fpi)) != EOF)
                {
                *p++ = (char)kar;
                assert(p - alltext < filesize+1);
                }
            *p = '\0';
            ::rewind(fpi);
            }
#endif
        }
    if(alltext)
        {
        parseAsXml();
        html_tag_class html_tag(this);
        CallBackEndAttributeNames = &XMLtext::CallBackEndAttributeNameCounting;
        if(element||ancestor)
            {
            html_tag.setCallBackStartElementName(::CallBackStartElementName);
            html_tag.setCallBackEndElementName(::CallBackEndElementName);
            html_tag.setCallBackEndTag(::CallBackEndTag);
            html_tag.setCallBackEmptyTag(::CallBackEmptyTag);
            }
        else
            {
            html_tag.setCallBackStartElementName(::dummyCallBack);
            html_tag.setCallBackEndElementName(::dummyCallBack);
            html_tag.setCallBackEndTag(::dummyCallBack);
            html_tag.setCallBackEmptyTag(::dummyCallBack);
            }
        if(wordAttribute||POSAttribute||PreTagAttribute)
            {
            html_tag.setCallBackStartAttributeName(::CallBackStartAttributeName);
            html_tag.setCallBackEndAttributeName(::CallBackEndAttributeName);
            }
        else
            {
            html_tag.setCallBackStartAttributeName(::dummyCallBack);
            html_tag.setCallBackEndAttributeName(::dummyCallBack);
            }
        html_tag.setCallBackStartValue(::dummyCallBack);
        html_tag.setCallBackEndValue(::dummyCallBack);
        html_tag.setCallBackNoMoreAttributes(::dummyCallBack);
        ch = alltext;
        char * curr_pos = alltext;
        char * endpos = alltext;
        estate Seq = notag;
#ifndef CONSTSTRCHR
        const 
#endif
        char * (wordReader::*fnc)(int kar);
        int loop;
        fnc = &wordReader::count;
        wordfield = 0;
        if(Iformat)
            {
            format = translateFormat(Iformat,wordfield);
            if(!wordfield)
                {
                printf("Input format %s must specify '$w'.\n",Iformat);
                exit(0);
                }
            }
        for(loop = 1;loop <= 2;++loop)
            {
            wordReader WordReader(format,wordfield,this);
            while(*ch)
                {
                while(  *ch 
                     && (( Seq = (html_tag.*tagState)(*ch)) == tag 
                        || Seq == endoftag_startoftag
                        )
                     )
                    {
                    ch++;
                    }
                if(Seq == notag)
                    { // Not an HTML tag. Backtrack.
                    endpos = ch;
                    ch = curr_pos;
                    while(ch < endpos)
                        {
                        (WordReader.*fnc)(*ch);
                        ch++;
                        }
                    }
                else if(Seq == endoftag)
                    ++ch; // skip >
                if(*ch)
                    {
                    while(  *ch 
                        && (Seq = (html_tag.*tagState)(*ch)) == notag
                        )
                        {
                        (WordReader.*fnc)(*ch);
                        ++ch;
                        }
                    (WordReader.*fnc)('\0');
                    if(Seq == tag)
                        {
                        (WordReader.*fnc)('\0');
                        curr_pos = ch++; // skip <
                        }
                    }
                }
            if(Seq == tag)
                { // Not an SGML tag. Backtrack.
                endpos = ch;
                ch = curr_pos;
                while(ch < endpos)
                    {
                    (WordReader.*fnc)(*ch);
                    ch++;
                    }
                }
            if(loop == 1)
                {
                numberOfTokens = total;
                Token = new token[total + 1]; // 1 extra for the epilogue after the last token

                total = 0;
                ch = alltext;
                curr_pos = alltext;
                fnc = &wordReader::read;
                CallBackEndAttributeNames = &XMLtext::CallBackEndAttributeNameInserting;
                if(wordAttribute||PreTagAttribute||POSAttribute)
                    {
                    html_tag.setCallBackStartAttributeName(::CallBackStartAttributeName);
                    html_tag.setCallBackEndAttributeName(::CallBackEndAttributeName);
                    html_tag.setCallBackStartValue(::CallBackStartValue);
                    html_tag.setCallBackEndValue(::CallBackEndValue);
                    html_tag.setCallBackNoMoreAttributes(::CallBackNoMoreAttributes);
                    }
                }
            }
        }
    }

