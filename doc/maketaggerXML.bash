METH=git
METH=https

if [ ! -d hashmap ]; then
    mkdir hashmap
    cd hashmap
    git init
    cd ..
fi
cd hashmap
git remote add origin $METH://github.com/kuhumcst/hashmap.git
git pull origin master
cd ..

if [ ! -d letterfunc ]; then
    mkdir letterfunc
    cd letterfunc
    git init
    cd ..
fi
cd letterfunc
git remote add origin $METH://github.com/kuhumcst/letterfunc.git
git pull origin master
cd ..

if [ ! -d parsesgml ]; then
    mkdir parsesgml
    cd parsesgml
    git init
    cd ..
fi
cd parsesgml
git remote add origin $METH://github.com/kuhumcst/parsesgml.git
git pull origin master
cd ..

if [ ! -d taggerXML ]; then
    mkdir taggerXML
    cd taggerXML
    git init
    cd ..
fi
cd taggerXML
git remote add origin $METH://github.com/kuhumcst/taggerXML.git
git pull origin master
cd src
make all
cd ..
cd ..
