#ifndef _USEFUL_H
#define _USEFUL_H

#include "sysdep.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE (!NULL)
#endif

#ifndef FALSE
#define FALSE NULL
#endif

#ifndef USHORT
#define USHORT unsigned short
#endif

#ifndef ULONG
#define ULONG unsigned long
#endif

#ifndef MAX
#define MAX(x, y) (x >= y ? x : y)
#endif

#ifndef MIN
#define MIN(x, y) (x <= y ? x : y)
#endif

#ifndef ABS
#define ABS(x) (x < 0 ? x * (-1) : x)
#endif

extern int strcmp_nocase( const char *string1, const char *string2 );
//extern const char * uncapitalize(const char * word);
extern char * dupl(const char * s);
extern char * mystrdup(const char *);
     /* Just a hack around the fact that strdup isn't standard */
extern int not_just_blank(char *);
    /* make sure we aren't processing a blank line */
extern int num_words(char *);
    /* returns number of words in a string */

/* Set 'product' to the name of the environment variable that contains (part
   of the) path to the options file, not including the file name itself.
   If NULL, a file named 'xoptions' is assumed to be in the current working
   directory. 
   Default value: NULL*/
extern char * product;
/* Set 'relative_path_to_xoptions' to the path to the options file, including
   the file name itself and relative to the value stored in the <product>
   environment variable. 
   If NULL, a file named 'xoptions' is assumed to be in the directory
   specified in the <product> environment varaible. (or in the CWD, if 
   product == NULL.
   Default value: NULL */
extern char * relative_path_to_xoptions;
extern char * xoptions;

long option(const char * key); 
char * coption(const char * key); 

// Bart 20030225:
/*
const char * allToLower(const char * s);
void AllToLower(char * s);
void AllToUpper(char * s);
void toLower(char * s);
void toUpper(char * s);
*/
//int isUpper(const char * s);
/*
bool isAllUpper(const char * s);
//bool isSpace(int s);
//bool isAlpha(int s);
extern const bool space[256];
extern const bool alpha[256];
extern const unsigned char upperEquivalent[256];
extern const unsigned char lowerEquivalent[256];
#define isSpace(s) space[(int)(s) & 0xFF]
#define isAlpha(s) alpha[(int)(s) & 0xFF]

#define isUpper(s) (upperEquivalent[(int)(*s & 0xFF)] == (int)(*s & 0xFF))
#define Upper(k) upperEquivalent[(int)(k & 0xFF)]
#define Lower(k) lowerEquivalent[(int)(k & 0xFF)]
*/
extern int allocated;
//#define INCREMENT allocated += 1;
//#define DECREMENT allocated -= 1;
#define INCREMENT
#define DECREMENT
#endif
