#ifndef DEFINES_H
#define DEFINES_H

#if defined __BORLANDC__
#define CONSTSTRCHR
/* Borland C++ 5.02 defines
const char *strchr(const char *s, int c);
char *strchr( char *s, int c);

Standard C++ defines 
char *strchr( const char *string, int c );

The Borland prototype is more picky and forces other functions to return
non-const character strings.
*/
#elif defined _MSC_VER
#if _MSC_VER >= 1400
#define CONSTSTRCHR
#endif
#endif


#define RESTRICT_MOVE 1   /* if this is set to 1, then a rule "change a tag 
			     from x to y" will only apply to a word if:
			     a) the word was not in the training set or
			     b) the word was tagged with y at least once in
			     the training set  
			     When training on a very small corpus, better
			     performance might be obtained by setting this to
			     0, but for most uses it should be set to 1 */
#define WITHSEENTAGGING 0 // Bart Jongejan 20050815. Save a hash table and some time on reading a large lexicon.
#define WITHWORDS 0 // Bart Jongejan 20050816. Save a hash table and some time on reading a large lexicon.
// It should make no difference wether one chooses WITHSEENTAGGING 0 and WITHWORDS 0 or WITHSEENTAGGING 1 and WITHWORDS 1.
// If it does, the lexicon probably has multiple lines for the same word, which is an error. (Bart 20090831)
#define MAXTAGLEN 256  /* max char length of pos tags */
#define MAXWORDLEN 256 /* max char length of words */
#define MAXAFFIXLEN 5  /* max length of affixes being considered */
#define STREAM 0

#endif
