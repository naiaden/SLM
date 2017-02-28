/*
 * TextPreprocessor.cpp
 *
 *  Created on: 28 feb. 2017
 *      Author: louis
 */

#include "TextPreprocessor.h"

#include <algorithm>

namespace SLM {

TextPreprocessor::TextPreprocessor() {
	// TODO Auto-generated constructor stub

}

TextPreprocessor::~TextPreprocessor() {
	// TODO Auto-generated destructor stub
}

CGNTextPreprocessor::CGNTextPreprocessor() {
	fillers = {"ggg",
			   "ah",
			   "bah",
			   "goh",
			   "ha",
			   "hej",
			   "hè",
			   "joh",
			   "mm-hu",
			   "oeh",
			   "oh",
			   "pff",
			   "tiens",
			   "tja",
			   "uh",
			   "wauw",
			   "weh",
			   "zuh",
			   "zulle",
			   "<UNK>",
			   "<sil>"};
}

CGNTextPreprocessor::~CGNTextPreprocessor() {
	// TODO Auto-generated destructor stub
}

bool CGNTextPreprocessor::isFiller(const std::string& w)
{
	auto it = std::find(fillers.begin(), fillers.end(), w);
	return it != fillers.end();
}

std::vector<std::string> CGNTextPreprocessor::removeFillers(std::vector<std::string>& words)
{
	words.erase(std::remove_if(words.begin(),
							   words.end(),
	                           [](std::string x){return isFiller(x);}),
			words.end());

	return words;
}

} /* namespace SLM */
