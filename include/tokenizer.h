/* 
 * File:   tokenizer.h
 * Author: wouter
 *
 * Created on August 6, 2010, 3:37 PM
 */

#ifndef _TOKENIZER_H
#define _TOKENIZER_H

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

#endif /* _TOKENIZER_H */

