#ifndef _HLTOKENIZER_H_
#define _HLTOKENIZER_H_

namespace valve
{

class Tokenizer
{
public:
    Tokenizer(const char* data, int size);
    Tokenizer(const Tokenizer& orig);
    virtual ~Tokenizer();

    const char* getToken();
    const char* getNextToken();
    bool nextToken();

public:
    static bool isSeperator(char c);
    static bool isQuote(char c);

private:
    const char* data;
    int dataSize;
    int cursor;
    char* token;
    int tokenSize;

};

}

#endif // _HLTOKENIZER_H_

