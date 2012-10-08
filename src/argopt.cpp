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

#include "argopt.h"
#include <string.h>
#include <stdio.h>

char *oldoptarg;
int oldoptind = 0;

int getopt(int argc,char *argv[],char *opts)
    {
    static char emptystr[] = "";

    char *index;
    int optc;
    
    if (!oldoptind)    /* argv[0] points to the command verb */
        ++oldoptind;
    if (oldoptind >= argc)
        {
        oldoptarg = NULL;
        return -1;
        }
    
    if ((index = argv[oldoptind]) != NULL)
        {
        char * optpos;
        if (*index != '-' 
#if defined __MSDOS__ || defined _WIN32			//if PC version
            && *index != '/'
#endif
            )
            {
            /* no option, perhaps something else ? */
            oldoptarg = NULL;
            return -1;
            }
        if (*(++index) == '-')
            {
            ++oldoptind;            /* double --,  end of options */
            oldoptarg = NULL;
            return -1;
            }
        if (!*index)
            {
                                /* single -, probably not an option */
            oldoptarg = NULL;
            return -1;
            }
        optc = *index;       /* option letter */
        optpos = strchr(opts,optc);
        if(optpos)
            {
            if(optpos[1] == ':')
                {
                /* this option has always data */
                if(!*++index)
                    {
                    /* try next argument */
                    for (;++oldoptind < argc && !*argv[oldoptind];);
                    if(  oldoptind == argc
                      || argv[oldoptind] == NULL
                      || (   *argv[oldoptind] == '-'
                         && *(argv[oldoptind]+1) != '\0'
                         )
                      )
                        {
                        oldoptarg = emptystr;
                        return optc;  /* no data after all */
                        }
                    else
                        {
                        oldoptarg = argv[oldoptind++]; 
                        if(oldoptarg[strlen(oldoptarg) - 1] == '\r') // Bart 20030806 Last argument has trailing '\r' under Linux !
                            oldoptarg[strlen(oldoptarg) - 1] = '\0';
                        return optc;
                        }
                    }
                else
                    {
                    oldoptind++;
                    oldoptarg = index; 
                    if(oldoptarg[strlen(oldoptarg) - 1] == '\r') // Bart 20030806 Last argument has trailing '\r' under Linux !
                        oldoptarg[strlen(oldoptarg) - 1] = '\0';
                    return optc;
                    }
                }
            else
                {
                oldoptind++;
                oldoptarg = NULL; 
                return optc;
                }
            }
        else
            {
            oldoptind++;
            oldoptarg = NULL; 
            return -1;
            }
        }
    else
        {
        oldoptind++;
        oldoptarg = NULL;
        return -1;
        }
    }
