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

HlTokenizer::HlTokenizer(const char* data, int size)
    : data(data), dataSize(size), cursor(0), token(0), tokenSize(0)
{ }

HlTokenizer::HlTokenizer(const HlTokenizer& orig)
    : data(orig.data), dataSize(orig.dataSize), cursor(0), token(0), tokenSize(0)
{ }

HlTokenizer::~HlTokenizer()
{
    if (this->token)
        delete []this->token;
}

const char* HlTokenizer::getToken()
{
    return this->token;
}

const char* HlTokenizer::getNextToken()
{
    if (this->nextToken())
        return this->getToken();

    return 0;
}

bool HlTokenizer::nextToken()
{
    // Check if we are at the end
    if (cursor >= this->dataSize)
        return false;

    // Reset the token
    if (this->token != 0)
        delete []this->token;
    this->token = 0;

    // Trim to the token
    while (cursor < this->dataSize && HlTokenizer::isSeperator(data[cursor]))
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
        while (cursor < this->dataSize && HlTokenizer::isSeperator(data[cursor]))
            cursor++;

        // Make sure we stop when the cursor is hits the end of the file
        if (cursor >= this->dataSize)
            return false;
    }

    // Are we at a quoted token?
    if (HlTokenizer::isQuote(data[cursor]))
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
        while (HlTokenizer::isSeperator(data[cursor + c]) == false && cursor < this->dataSize)
            c++;
        this->token = new char[c + 2];
        memcpy(this->token, data + cursor, c);
        this->token[c] = 0;
        cursor+= c;
    }

    return true;
}

bool HlTokenizer::isSeperator(char c)
{
    if (c <= ' ') return true;

    return false;
}

bool HlTokenizer::isQuote(char c)
{
    if (c == '\"') return true;
    if (c == '\'') return true;

    return false;
}
