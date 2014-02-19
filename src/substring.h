#ifndef SUBSTRING_H
#define SUBSTRING_H

class substring
    {
    private:
        char * start;
        char * end;
    public:
        static char * savepos;
        static char savechar;
        static void reset()
            {
            if(savepos)
                {
                *savepos = savechar;
                savepos = 0;
                }
            }
        const char * itsWord() const
            {
            reset();
            savepos = end;
            savechar = *savepos;
            *savepos = '\0';
            return start;
            }
        substring():start(0),end(0)
            {
            }
        void set(substring & other)
            {
            this->start = other.start;
            this->end = other.end;
            }
        void set(char * start,char * end)
            {
            this->start = start;
            this->end = end;
            }
        char * getStart() const
            {
            return start;
            }
        char * getEnd() const
            {
            return end;
            }
    };

#endif
