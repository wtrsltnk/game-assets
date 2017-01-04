/*
 * Wellicht handig dit een keer te gebruiken:
 *
 *
try
{
    std::string s = (const char*)this->entityData;
    std::smatch m;

    std::regex e = std::regex("\\{([^\\}]*)\\}");
    std::regex ee = std::regex("\"([^\"]*)\"");

    while (std::regex_search(s, m, e))
    {
        std::string ent = s.substr(m.position(), m.length());
        while (std::regex_search(ent, m, ee))
        {
            std::cout << m.str() << std::endl;
            ent = m.suffix().str();
        }

        s = m.suffix().str();
    }
}
catch (std::regex_error& err)
{
    std::cout << err.what() << std::endl;
}
*/

#include "valve/hltokenizer.h"

#include <stdlib.h>
#include <string.h>

using namespace valve;

Tokenizer::Tokenizer(const char* data, int size)
    : data(data), dataSize(size), cursor(0), token(0), tokenSize(0)
{ }

Tokenizer::Tokenizer(const Tokenizer& orig)
    : data(orig.data), dataSize(orig.dataSize), cursor(0), token(0), tokenSize(0)
{ }

Tokenizer::~Tokenizer()
{
    if (this->token)
        delete []this->token;
}

const char* Tokenizer::getToken()
{
    return this->token;
}

const char* Tokenizer::getNextToken()
{
    if (this->nextToken())
        return this->getToken();

    return 0;
}

bool Tokenizer::nextToken()
{
    // Check if we are at the end
    if (cursor >= this->dataSize)
        return false;

    // Reset the token
    if (this->token != 0)
        delete []this->token;
    this->token = 0;

    // Trim to the token
    while (cursor < this->dataSize && Tokenizer::isSeperator(data[cursor]))
        cursor++;

    // Check if we are at the end
    if (cursor >= this->dataSize)
        return false;

    int c = 0;

    if (data[cursor] == '/' && data[cursor+1] == '/')
    {
        while (cursor + c < this->dataSize && data[cursor] != '\n')
            cursor++;

        // Trim to the token
        while (cursor < this->dataSize && Tokenizer::isSeperator(data[cursor]))
            cursor++;

        // Make sure we stop when the cursor is hits the end of the file
        if (cursor >= this->dataSize)
            return false;
    }

    // Are we at a quoted token?
    if (Tokenizer::isQuote(data[cursor]))
    {
        while (data[cursor] != data[cursor + c + 1] && data[cursor + c + 1] != 0 && cursor < this->dataSize)
            c++;
        this->token = new char[c + 2];
        memcpy(this->token, data + cursor + 1, c);
        this->token[c] = 0;
        cursor+= c + 2;
    }
    else
    {
        while (Tokenizer::isSeperator(data[cursor + c]) == false && cursor < this->dataSize)
            c++;
        this->token = new char[c + 2];
        memcpy(this->token, data + cursor, c);
        this->token[c] = 0;
        cursor+= c;
    }

    return true;
}

bool Tokenizer::isSeperator(char c)
{
    if (c <= ' ') return true;

    return false;
}

bool Tokenizer::isQuote(char c)
{
    if (c == '\"') return true;
    if (c == '\'') return true;

    return false;
}
