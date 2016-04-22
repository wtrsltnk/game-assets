#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

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

#endif // _TOKENIZER_H_

