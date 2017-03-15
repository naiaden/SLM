/*
 * TextPreprocessor.h
 *
 *  Created on: 28 feb. 2017
 *      Author: louis
 */

#ifndef TEXTPREPROCESSOR_H_
#define TEXTPREPROCESSOR_H_

#include <vector>
#include <string>

namespace SLM {

class TextPreprocessor {
public:
	TextPreprocessor();
	virtual ~TextPreprocessor();
};

class CGNTextPreprocessor : public TextPreprocessor {
public:
	CGNTextPreprocessor();
	virtual ~CGNTextPreprocessor();

	std::vector<std::string> removeFillers(std::vector<std::string>& words, bool removeMarkers = false);
protected:
	static bool isFiller(const std::string& word);

	static std::vector<std::string> fillers;

};

} /* namespace SLM */

#endif /* TEXTPREPROCESSOR_H_ */
