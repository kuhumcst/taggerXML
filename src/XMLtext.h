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

#ifndef XMLTEXT_H
#define XMLTEXT_H

#include "text.h"

class crumb;

class XMLtext : public text
    {
    private:
        char * startElement;
        char * endElement;
        char * startAttributeName;
        char * endAttributeName;
        char * startValue;
        char * endValue;
        const char * ancestor; // if not null, restrict POS-tagging to elements that are offspring of ancestor
        const char * segment; // Optional segment delimiter. E.g. br: <br /> or s: <s> ... </s>
        const char * element; // if not null, analyse only element's attributes and/or PCDATA
        const char * PreTagAttribute; // if null, POS is PCDATA
        const char * POSAttribute; // Store POS in POSAttribute
        int wordAttributeLen;
        int PreTagAttributeLen;
        int POSAttributeLen;
        crumb * Crumbs;
        bool ClosingTag;
        bool WordPosComing;
        bool PreTagPosComing;
        bool POSPosComing;
    public:
        virtual const char * convert(const char * s);
        const char * wordAttribute; // if null, word is PCDATA
    public:
        bool analyseThis();
        token * getCurrentToken();
        void CallBackStartElementName();
        void CallBackEndElementName();
        void CallBackStartAttributeName();
        void CallBackEndAttributeNameInserting();
        void CallBackEndAttributeNameCounting();
        void CallBackStartValue();
        void CallBackEndValue();
        void CallBackEndTag();
        void CallBackEmptyTag();
        void CallBackNoMoreAttributes();
        XMLtext(
#if STREAM
            istream & fpi
#else
            FILE * fpi
#endif
           ,const char * Iformat
//           ,bool nice
//           ,unsigned long int size
           ,bool XML
           ,const char * ancestor // if not null, restrict POS-tagging to elements that are offspring of ancestor
           ,const char * segment // Optional segment delimiter. E.g. br: <br /> or s: <s> ... </s>
           ,const char * element // analyse PCDATA inside elements
           ,const char * wordAttribute // if null, word is PCDATA
           ,const char * PreTagAttribute // Fetch pretagging from PreTagAttribute
           ,const char * POSAttribute // Store POS in POSAttribute
            );
        ~XMLtext(){}
        virtual void printUnsorted(
#if STREAM
            ostream & fpo
#else
            FILE * fpo            
#endif
            );
    };

#endif
