/*
 * TextPreprocessor.cpp
 *
 *  Created on: 28 feb. 2017
 *      Author: louis
 */

#include "TextPreprocessor.h"

#include <algorithm>

namespace SLM {

std::vector<std::string> CGNTextPreprocessor::fillers ={"ggg", "ah", "bah", "goh", "ha", "hej", "h�", "joh", "mm-hu", "oeh", "oh", "pff", "tiens", "tja", "uh", "wauw", "weh", "zuh", "zulle", "<UNK>", "<sil>"};

TextPreprocessor::TextPreprocessor() {
	// TODO Auto-generated constructor stub

}

TextPreprocessor::~TextPreprocessor() {
	// TODO Auto-generated destructor stub
}

CGNTextPreprocessor::CGNTextPreprocessor() {

}

CGNTextPreprocessor::~CGNTextPreprocessor() {
	// TODO Auto-generated destructor stub
}

bool CGNTextPreprocessor::isFiller(const std::string& w)
{
	auto it = std::find(SLM::CGNTextPreprocessor::fillers.begin(), SLM::CGNTextPreprocessor::fillers.end(), w);
	return it != SLM::CGNTextPreprocessor::fillers.end();
}

std::vector<std::string> CGNTextPreprocessor::removeFillers(std::vector<std::string>& words, bool removeMarkers)
{
	words.erase(std::remove_if(words.begin(),
							   words.end(),
	                           [](const std::string& x){return SLM::CGNTextPreprocessor::isFiller(x);}),
				words.end());

	if(removeMarkers)
	{
		words.erase(std::remove_if(	words.begin(),
									words.end(),
			                        [](const std::string& x){return x == "<s>" || x == "</s>";}),
						words.end());
	}

	return words;
}

} /* namespace SLM */
