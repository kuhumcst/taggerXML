#ifndef _LEX_H_
#define _LEX_H_


extern char *append_with_space(char *,char *); 
    /* takes 2 strings, and appends them with a space btwn them */

extern char *append_with_char(char *,char *,char);
             /* takes 2 strings, and appends them with character w3 (the */
	     /* third argument) btwn them */

extern char **perl_split(char *);
	      /* takes a string with spaces and does a split, returning */
	      /* an array of ptrs to strings. */
              /* like perl - @temp = split(/\s+/,$buf); 
		 Last ptr is a null. */
     /* x = perl_split(buf);  then you are responsible for freeing
	*x and x */

extern char **perl_split_independent(char *);
         /* same as perl_split, but each element in the array is a separate */
	 /* string of memory. */
#if 0
extern char **perl_split_on_char(char *,char);
        /* same as perl_split, but split on the specified character, instead */
	/* of space */
#endif
extern char **perl_split_on_nothing(char *);
         /* same as perl_split, but split on nothing instead of space*/



extern char *return_tag(char *);   /* returns a ptr to the tag
				in a tagged word (the/dt),
				or NULL if not tagged.*/

#endif /* _LEX_H_ */
