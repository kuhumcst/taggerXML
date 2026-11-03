#taggerXML#
Modernized version of Eric Brill's Part Of Speech tagger.
The original code is converted to C++.
The program can handle XML input.
This repository also contains the code for training the tagger with a tagged
corpus of your own choice (see **Training** below).

You can find a copy of Eric Brill's original tagger here:

    [cst.dk/download/tagger/RBT1_14.tar.Z](http://cst.dk/download/tagger/RBT1_14.tar.Z)

This file contains linguistic resources for tagging English text as well.

**Installation**

* Linux:
    1. Download (e.g. git pull) taggerXML, parsesgml, letterfunc and hashmap. If you are going to use the Makefile that comes with taggerXML, locate each of these packages in separate subdirectories under the same directory, and call these subdirectories taggerXML, parsesgml, letterfunc and hashmap, respectively.
    2. Change directory to the 'taggerXML/src' directory.
    3. Run 'make' or 'make taggerXML'. To get rid of object files, run 
    4. 'make clean'.

You can use https://github.com/kuhumcst/taggerXML/blob/master/doc/maketaggerXML.bash to do all of this automatically.

**Running**

For running the taggerXML, see Eric Brill's original documentation and https://github.com/kuhumcst/taggerXML/blob/master/doc/improvements.md

**Training**

Training needs perl and the programs taggerXML and contextual-rule-learn.
'make all' in the 'src' directory builds them into the 'Bin_and_Data' directory.

The training code consists of:

* `trainBrill.bash` - runs all training steps in one go.
* `Learner_Code/` - the lexical rule learners for unknown words
  (unknown-lexical-learn.prl, and unknown-lexical-learn-continue.prl for
  resuming an interrupted run). The contextual rule learners
  (contextual-rule-learn, kbest-contextual-rule-learn, fix-kbest-rule-learn)
  are in `src/`.
* `Utilities/` - Brill's Perl scripts for preparing the training data.
* `doc/README.TRAINING` - Brill's description of the training procedure.

The input is a tagged corpus with one sentence per line and tokens written as
word/TAG. Example:

    ./trainBrill.bash -o model -n N_INDEF_SING -p EGEN -s 42 corpus.txt

The -n and -p options give the tags that unknown words get in the start state:
-p for words starting with an upper case letter, -n for all other words. Use
the same tags when you run taggerXML with the trained files:

    taggerXML -n N_INDEF_SING -p EGEN model/FINAL.LEXICON text model/BIGBIGRAMLIST model/LEXRULEOUTFILE model/CONTEXT-RULEFILE

Learning the lexical rules is slow. If it gets interrupted, rerun with -r to
continue where it stopped. Run './trainBrill.bash -h' for all options.

**Online availability**

taggerXML is demonstrated at CST's website
(http://ada.sc.ku.dk/tools/index.php?lang=en)
and an integrated webservice in the CLARIN-DK infrastructure
(https://clarin.dk/).

**Contact info**

For questions and remarks about the program, please feel free to contact us.

Our postal address is:

    Center for Sprogteknologi
    University of Copenhagen
    Njalsgade 140
    2300 Copenhagen S.
    Denmark

On the internet, you can visit us at [www.cst.ku.dk](http://www.cst.ku.dk)
Here you can also try the tagger for Danish and English.

