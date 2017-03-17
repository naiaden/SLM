/*
 * WordList.h
 *
 *  Created on: Mar 17, 2017
 *      Author: louis
 */

#ifndef TOOLS_WORDLIST_H_
#define TOOLS_WORDLIST_H_

#include <unordered_set>

#include "FileReader.h"

namespace SLM {

class WordList : public SLM::FileReader {
public:
	WordList(const std::string& inputDir, const std::string& file);
	virtual ~WordList();

	bool contains(const std::string& word) const;
protected:
	std::unordered_set<std::string> wordList;
};

} /* namespace SLM */

#endif /* TOOLS_WORDLIST_H_ */
