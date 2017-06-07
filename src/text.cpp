#include "field.h"
#include "text.h"
#include "sst.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "hashmap.h"

static hashmap::hash<strng> * Hash = 0;


const char * token::getWord()
    {
    substring::reset();
    if(this->wordPrePos)
        {
        return wordPrePos->key();
        }
    else
        return this->Wordpos.getStart();
    }


const char * 
#if STREAM
copy(ostream & fp,const char * o,const char * end)
    {
    for(;o < end;++o)
        fp << *o;
    return o;
    }
#else
copy(FILE * fp,const char * o,const char * end)
    {
    for(;o < end;++o)
        fputc(*o,fp);
    return o;
    }
#endif


#ifdef COUNTOBJECTS
int text::COUNT = 0;
#endif

void text::AddField(field * fld)
    {
    if(fields == 0)
        fields = fld;
    else
        fields->addField(fld);
    }

field * text::translateFormat(const char * Iformat,field *& wordfield/*,field *& tagfield*/)
    {
    bool escape = false;
    bool afield = false;
    const char * pformat;
    field * litteral = 0;
    for(pformat = Iformat;*pformat;++pformat)
        {
        if(afield)
            {
            afield = false;
            switch(*pformat)
                {
                case 'w':
                    if(wordfield)
                        {
                        printf("Invalid format string \"%s\"\n",Iformat);
                        printf("                        %*c\n",(int)(strlen(Iformat) - strlen(pformat)),'^');
                        exit(0);
                        }
                    wordfield = new readValue();
                    litteral = 0;
                    AddField(wordfield);
                    break;
                /*case 't':
                    if(tagfield)
                        {
                        printf("Invalid format string \"%s\"\n",Iformat);
                        printf("                        %*c\n",(int)(strlen(Iformat) - strlen(pformat)),'^');
                        exit(0);
                        }
                    tagfield = new readValue();
                    litteral = 0;
                    AddField(tagfield);
                    break;*/
                case 'd':
                    litteral = 0;
                    AddField(new readValue());
                    break;
                default:
                    {
                    printf("Invalid format string \"%s\"\n",Iformat);
                    printf("                        %*c\n",(int)(strlen(Iformat) - strlen(pformat)),'^');
                    exit(0);
                    }
                }
            }
        else if(escape)
            {
            escape = false;
            switch(*pformat)
                {
                case 's':
                    litteral = 0;
                    AddField(new readWhiteSpace);
                    break;
                case 'S':
                    litteral = 0;
                    AddField(new readAllButWhiteSpace);
                    break;
                case 't':
                    litteral = 0;
                    AddField(new readTab);
                    break;
                case 'n':
                    litteral = 0;
                    AddField(new readNewLine);
                    break;
                default:
                    {
                    printf("Invalid format string \"%s\"\n",Iformat);
                    printf("                        %*c\n",(int)(strlen(Iformat) - strlen(pformat)),'^');
                    exit(0);
                    }
                }
            }
        else if(*pformat == '\\')
            {
            escape = true;
            }
        else if(*pformat == '$')
            {
            afield = true;
            }
        else
            {
            if(!litteral)
                {
                litteral = new readLitteral(*pformat);
                AddField(litteral);
                }
            else
                litteral->add(*pformat);
            }
        }
    return fields;
    }

strng * text::insert(const char * w)
    {
    w = convert(w); // XML-entities -> characters
    if(!Hash)
        {
        Hash = new hashmap::hash<strng>(&strng::key,1000);
        }
    void * v = NULL;
    strng * wrd = Hash->find(w,v);
    if(!wrd)
        {
        wrd = new strng(w,0);
        Hash->insert(wrd,v);
        }
    CurrToken = Token + total;
    ++total;
    return wrd;
    }


#if ESCAPESLASH
const char * text::convert(const char * s)
    {
    const char * p = strchr(s,'\\');
    if(p && p[1])
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
            if(*s == '\\' && s[1])
                {
                ++s;
                }
            *p++ = *s++;
            }
        *p = '\0';
        return ret[index];
        }
    return s;
    }
#endif


text::text(/*bool nice*/)
           :alltext(NULL)
           ,total(0)
           ,fields(0)
    {
#ifdef COUNTOBJECTS
    ++COUNT;
#endif
    }

#if STREAM
text::text(istream & fpi,bool FINAL_ONLY_FLAG)
#else
text::text(FILE * fpi,bool FINAL_ONLY_FLAG)
#endif
           :alltext(NULL)
           ,total(0)
           ,fields(0)
    {
#ifdef COUNTOBJECTS
    ++COUNT;
#endif
    
#if STREAM
    fpi.seekg(0,ios::end);
    long filesize = fpi.tellg();
    fpi.seekg(0,ios::beg);
#else
    fseek(fpi,0,SEEK_END);
    long filesize = ftell(fpi);
    ::rewind(fpi);
#endif
    bool intoken = false;
    bool inaline = false;
    numberOfTokens = 0;
    if(filesize > 0)
        {
        alltext = new char[filesize+1];
        char * p = alltext;
#if STREAM
        char * e = alltext + filesize;
        char kar;
        while(p < e)
            {
            fpi >> kar;
#else
        int kar;
        while((kar = getc(fpi)) != EOF)
            {
#endif
            if(intoken)
                {
                switch(kar)
                    {
                    case '\n':
                    case '\r':
                    case ' ':
                    case '\t':
                        intoken = false;
                        break;
                    default:
                        ;
                    }
                }
            else
                {
                switch(kar)
                    {
                    case '\n':
                    case '\r':
                    case ' ':
                    case '\t':
                        break;
                    default:
                        intoken = true;
                        ++numberOfTokens;
                    }
                }
            *p++ = (char)kar;
            }
        *p = '\0';
        Token = new token[numberOfTokens + 1]; // 1 extra for the epilogue after the last token
        p = alltext;
        numberOfTokens = 0;
        char * beginpos = alltext;
        char * endpos = alltext;
        token * Tok = NULL;
        bool tokenset = false;
        intoken = false;
        for(p = alltext;*p;++p)
            {
            if(intoken)
                {
                switch(*p)
                    {
                    case '\n':
                    case '\r':
                        Tok->lastOfLine = true;
                        inaline = false;
                        /* drop through */
                    case ' ':
                    case '\t':
                        {
                        endpos = p;
#if ESCAPESLASH
                        bool esc = false;
#else
                        int slashes = 0;
#endif
                        for(p = beginpos;p < endpos;++p)
                            {
#if ESCAPESLASH
                            if(*p == '\\')
                                {
                                esc = true;
                                }
                            else
                                {
                                if(esc)
                                    esc = false;
                                else if(*p == '/')
                                    {
                                    /* a word can be pretagged by putting a slash between the */
                                    /* word and the tag ::  The boy/NN ate . */
                                    /* if a word is pretagged, we just spit out the pretagging */
                                    Tok->setWord(beginpos,p,this);
                                    Tok->setPreTagpos(p+1,endpos);
                                    Tok->Pos = Tok->PreTag = new char[endpos - p];
                                    strncpy(Tok->PreTag,p+1,endpos - p - 1);
                                    Tok->PreTag[endpos - p - 1] = '\0';
                                    tokenset = true;
                                    p = endpos;
                                    break;
                                    }
                                }
#else
                            if(*p == '/')
                                {
                                ++slashes;
                                if(slashes == 2)
                                    {
                /* a word can be pretagged by putting two slashes between the */
                /* word and the tag ::  The boy//NN ate . */
                /* if a word is pretagged, we just spit out the pretagging */
                                    Tok->setWord(beginpos,p-1,this);
                                    Tok->setPreTagpos(p+1,endpos);
                                    Tok->Pos = Tok->PreTag = new char[endpos - p];
                                    strncpy(Tok->PreTag,p+1,endpos - p - 1);
                                    Tok->PreTag[endpos - p - 1] = '\0';
                                    tokenset = true;
                                    p = endpos;
                                    break;
                                    }
                                }
                            else
                                {
                                if(slashes && FINAL_ONLY_FLAG)
                                    {
                                    Tok->setWord(beginpos,p-1,this);
                                    Tok->setPreTagpos(p,endpos);
                                    Tok->Pos = Tok->PreTag = new char[endpos - p + 1];
                                    strncpy(Tok->PreTag,p,endpos - p);
                                    Tok->PreTag[endpos - p] = '\0';
                                    tokenset = true;
                                    p = endpos;
                                    break;
                                    }
                                slashes = 0;
                                }
#endif
                            }
                        if(!tokenset)
                            Tok->setWord(beginpos,endpos,this);
                        intoken = false;
                        break;
                        }
                    default:
                        ;
                    }
                }
            else
                {
                switch(*p)
                    {
                    case '\n':
                    case '\r':
                        if(Tok) /* Tok is still NULL if the text starts with an empty line. */
                            Tok->lastOfLine = true;
                        inaline = false;
                        /* drop through */
                    case ' ':
                    case '\t':
                        break;
                    default:
                        intoken = true;
                        beginpos = p;
                        Tok = Token + numberOfTokens++;
                        tokenset = false;
                        if(!inaline)
                            {
                            inaline = true;
                            Tok->firstOfLine = true;
                            }
                    }
                }
            }
        if(intoken)
            Tok->setWordpos(beginpos,p);
        Token[numberOfTokens].setWordpos(p,p);
        }
    }

    int X = 0;
    token::token():wordPrePos(0),PreTag(0),Pos(0),firstOfLine(false),lastOfLine(false)
        {
        ++X;
        }

    void token::setWord(char * start,char * end,text * Text)
        {
        setWordpos(start,end);
        char sav = 0;
        if(*end)
            {
            sav = *end;
            *end = 0;
            }
        wordPrePos = Text->insert(start);
        if(sav)
            *end = sav;
        }


void text::printUnsorted(
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
            if(start)
                {
                copy(fpo,o,end);
                o = Token[k].PreTagGetEnd();
                if(!o)
                    o = end;
                }
            char * p = Token[k].Pos;
            if(p)
                {
#if STREAM
                fpo << '/';
#else
                fputc('/',fpo);
#endif

                while(*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r')
#if STREAM
                    fpo << *p++;
#else
                    fputc(*p++,fpo);
#endif
                }
            }
        o = copy(fpo,o,o+strlen(o));
        }
    }


    
text::~text()
    {
    if (Hash) { Hash->deleteThings(); delete Hash; }
    if (alltext) delete[] alltext;
    delete fields;
#ifdef COUNTOBJECTS
    --COUNT;
#endif
    }

