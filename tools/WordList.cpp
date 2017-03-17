/*
 * WordList.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: louis
 */

#include "WordList.h"

#include "Logging.h"

namespace SLM {

WordList::WordList(const std::string& fileName) : FileReader(fileName) {
	std::string line;
	if (file.is_open())
	{
		while ( getline (file,line) )
		{ // std::vector<std::string>(entries.begin() + 3, entries.end()) ;
			wordList.emplace(line);
		}
		file.close();
	}

	L_V << "Wordlist: Read " << wordList.size() << " words from " << fileName << "\n";
}

WordList::~WordList() {
	// TODO Auto-generated destructor stub
}


bool WordList::contains(const std::string& word) const
{
	return wordList.count(word)>0;
}


} /* namespace SLM */
