#taggerXML#
Modernized version of Eric Brill's Part Of Speech tagger.
The original code is converted to C++.
The program can handle XML input.
This program can only tag. For training the tagger with a tagged corpus of your
own choice you can use Eric Brill's original software.

You can find a copy of Eric Brill's tagger here:

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

