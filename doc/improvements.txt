*CST's version of Brill's Tagger*

**Highlights**

* Source code is in Standard C and platform independent. (No need for 
    libraries that only exist in *n?x.)
* Improved handling of capital letters.
* Modernized (but backwards compatible) arguments using option letters

**Handling of capital letters**

The program checks whether the majority of words in a line start with a
capital letter. If that is the case, the assumption is that the line
is a head-line. All words are lowercased.

If the first word of a line starts with a capital letter, lexicon look-up is
done for the word with capital letter and also for the lower-cased version of
the word.


**Command line**

Type 

        taggerXML -h

Output:    
    
usage:
taggerXML [options] [LEXICON] [CORPUS-TO-TAG] [BIGRAMS] [LEXICALRULEFILE] [CONTEXTUALRULEFILE]
options:
    -@<optionsfile>
    -h   help
    -?   help
    -D<LEXICON>
    -i<CORPUS-TO-TAG>
    -B<BIGRAMS>
    -L<LEXICALRULEFILE>
    -C<CONTEXTUALRULEFILE>
    -w<WORDLIST>
    -m<INTERMEDFILE>
    -S   start state tagger only
    -F   final state tagger only
    -o<out> output (optional, otherwise stdout)
    -r   About redistribution (GNU)
    -W   About warranty (GNU)
    -x<path> path to file with extra options (deprecated)
    -f ConvertToLowerCaseIfFirstWord (default off)
    -a ConvertToLowerCaseIfMostWordsAreCapitalized (default off)
    -s ShowIfLowercaseConversionHelped (default off)
    -n<class> Noun (default NN)
    -p<class> Proper (default NNP)
    -v Verbose (default off)
============================
    -X  XML input. Leave XML elements unchanged.
    The next options do not allow space between option letters and argument!
    -Xa<ancestor>  Only analyse elements with specified ancestor. e.g -Xap
    -Xs<delimiter> Segment (sentence) delimiter. Can be empty tag. e.g -Xsbr or -Xss
    -Xe<element>  Only analyse specified element. e.g -Xew
    -Xw<word>  Words are to be found in attribute. e.g -Xwword
    -Xt<pretag>  Words' pre-tagging to be found in attribute. e.g -Xtprepos
    -Xp<POS>  Destination of POS is the specified attribute. e.g -Xppos


**xoptions**

You can still use the original command line syntax of the Brill tagger.
In the original command line syntax, an argument's role was dependent on the
argument's position on the command line:

tagger arg1 arg2 arg3 arg3 arg5
where:
    arg1: LEXICON 
    arg2: CORPUS-TO-TAG 
    arg3: BIGRAMS 
    arg4: LEXICALRULEFILE 
    arg5: CONTEXTUALRULEFILE

Without changing the command line, extra options can be set in the file 
xoptions. (This is a CST-extension to the original Brill tagger.) The
xoptions file is like this:

ConvertToLowerCaseIfFirstWord = 1
ConvertToLowerCaseIfMostWordsAreCapitalized = 1
ShowIfLowercaseConversionHelped = 0
Language = danish  (One of the LC_CTYPES accepted by setlocale(), e.g.
 "danish", "dutch", "english", "french", "german", "italian", "spanish")
Noun = N
Proper = EGEN
Verbose = 0

Lines that start with a blank are ignored, so the fifth line is ignored.
All other lines must have the following syntax:

keyword = value

or

keyword = value comments

Keywords are:

    ConvertToLowerCaseIfFirstWord
    ConvertToLowerCaseIfMostWordsAreCapitalized
    ShowIfLowercaseConversionHelped
    Language
    Noun
    Proper
    Verbose

The use of old style arguments and the xoptions file is deprecated. All
old style arguments and xoptions-options can also be passed to the tagger
by using option letters followed by option values. See above.

You can mix new style options and old style arguments and/or options.
If the xoptions file mentions an option, that option's value overrules a
new style option with the same role. For example, if the xoptions file says

ConvertToLowerCaseIfFirstWord = 0

and there also is an option -f, the first word of a line is not converted to
lower case.

**-@ parameter file**

You can put as many options in a text file as you like and pass the path to
the text file as the value of the -@ option. Each option is on a separate line,
while blank lines and lines starting with a semicolon are igored.
An example arg.txt:


;-v
; Lexicon
-D /var/csttools/res/web/da/tagger/UTF8/FINAL.LEXICON
; input
-i test-cbf-header-tok.xml
; bigramlist
-B /var/csttools/res/web/da/tagger/UTF8/BIGBIGRAMLIST
; lexial rules
-L /var/csttools/res/web/da/tagger/UTF8/LEXRULEOUTFILE
; contextual rules
-C /var/csttools/res/web/da/tagger/UTF8/CONTEXT-RULEFILE
; default noun-tag
-n N
; default proper noun-tag
-p EGEN
; Convert To Lower Case If First Word ?
-f
; Convert To Lower Case If Most Words Are Capitalized (headlines)? 
-a 
;Show If Lowercase Conversion Helped ? 
;-s 
; Xa: ancestor of elements that must be tagged      -XaspanGrp 
; Xe: elements that must be tagged  -Xespan
; Xs: elements that indicate sentence delimiters
;           (for example <p>...</p> el. <br />) 
;-Xsbr 
; Xp attribute where output is to be parked 
;-Xppos 
;Xt  attributte where existing PoS tags can be found
;-Xtana 
; Xw attributte where the word can be found (if it is not PCDATA of the element) 
;-Xwword


and call

    taggerXML -@ args.txt

You can overrule options in arg.txt by putting more options on the command
line after the -@ option:

    taggerXML -@ args.txt -i mytext -o mytext.tagged
    



You have to be careful with the -X option: if it hasn't got an argument you
are telling the tagger to let XML-tags be untouched, but to tag everything
else. Now if you write:

    taggerXML  -X FINAL.LEXICON input BIGBIGRAMLIST LEXRULEOUTFILE CONTEXT-RULEFILE

the string 'FINAL.LEXICON' is interpreted as -X'es argument. To circumvent
this unwanted side effect, you should write

    taggerXML  -X -D FINAL.LEXICON input BIGBIGRAMLIST LEXRULEOUTFILE CONTEXT-RULEFILE

If a new option comes after the X-option (-D in this exampel), this signals
to the X-option that it has no argument.

Bart Jongejan 20121009
